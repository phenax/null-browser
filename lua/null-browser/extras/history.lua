local history = {
  path = web.get('app_data_dir') .. '/.null-browser-history',
  max_entires = 200,
}

local hook_registered = false

function history.list()
  local file = uv.fs_open(history.path, 'r', 438)
  if not file then return {} end
  local stat = assert(uv.fs_fstat(file))
  local data = assert(uv.fs_read(file, stat.size))
  assert(uv.fs_close(file))

  local urls = {}
  for line in string.gmatch(data, '[^\r\n]+') do
    local already_exists = false
    for _, url in ipairs(urls) do
      if history.are_urls_similar(url, line) then already_exists = true end
    end
    if #urls >= history.max_entires then break end
    if not already_exists then
      table.insert(urls, line)
    end
  end

  return urls
end

function history.are_urls_similar(url1, url2)
  return url1 == url2 or url1 .. '/' == url2
end

function history.update(func)
  local urls = history.list()
  local file = assert(uv.fs_open(history.path, 'w', 438))
  urls = func(urls)
  local contents = table.concat(urls, '\n')
  assert(uv.fs_write(file, contents))
  assert(uv.fs_close(file))
end

function history.add(url)
  history.update(function(urls)
    table.insert(urls, 1, url)
    return urls
  end)
end

function history.delete(url_to_delete)
  history.update(function(urls)
    for index, url in ipairs(urls) do
      if url == url_to_delete then
        table.remove(urls, index)
        return urls
      end
    end
    return urls
  end)
end

function history.attach_hooks()
  if hook_registered then return end

  web.event.add_listener('UrlChanged', {
    callback = function(opts)
      print('url change', web.inspect(opts));
      history.add(opts.url)
    end
  })
end

return history
