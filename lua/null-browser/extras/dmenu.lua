local dmenu = {}

function dmenu.select(list, opts, callback)
  local selection = nil
  local stdin = web.uv.new_pipe();
  local stdout = web.uv.new_pipe();
  local args = opts.args or {}
  if opts.prompt then
    table.insert(args, '-p')
    table.insert(args, opts.prompt)
  end
  if opts.input then
    table.insert(args, '-it')
    table.insert(args, opts.input)
  end

  web.uv.spawn('dmenu', { args = args, stdio = { stdin, stdout, nil } }, function(code)
    web.uv.close(stdout)
    web.uv.close(stdin)
    if code == 0 then
      callback(nil, selection)
    else
      callback('Exit with status code: ' .. code, selection)
    end
  end)

  web.uv.read_start(stdout, function(_, data)
    if data then selection = data end
  end)

  for _, value in ipairs(list) do
    web.uv.write(stdin, value .. '\n')
  end
  web.uv.shutdown(stdin)
end

function dmenu.input(opts, callback)
  dmenu.select({}, opts, callback)
end

return dmenu
