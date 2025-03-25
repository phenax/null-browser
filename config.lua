print('hello starting up...')

--- @type table
web = web
--- @type table
uv = uv

local Dmenu = {}
function Dmenu.select(list, opts, callback)
  local selection = nil
  local stdin = uv.new_pipe();
  local stdout = uv.new_pipe();
  local args = { '-p', opts.prompt or '>' }
  uv.spawn('dmenu', { args = args, stdio = { stdin, stdout, nil } }, function(code)
    uv.close(stdout)
    uv.close(stdin)
    if code == 0 then
      callback(nil, selection)
    else
      callback('Exit with status code: ' .. code, selection)
    end
  end)

  uv.read_start(stdout, function(_err, data)
    if data then selection = data end
  end)

  for _, value in ipairs(list) do
    uv.write(stdin, value .. '\n')
  end
  uv.shutdown(stdin)
end

local function trim(s)
  return s:gsub("^%s*(.-)%s*$", "%1")
end

local urls = {
  'https://excalidraw.com',
  'https://lite.duckduckgo.com',
  'https://ediblemonad.dev',
  'https://google.com',
  'https://github.com/trending',
}

-- Open in new tab
web.keymap.set('n', 't', function()
  Dmenu.select(urls, { prompt = 'Open tab:' }, function(err, result)
    if err or not result then return end
    web.tabs.new(trim(result))
  end)
end)
-- Open in current tab
web.keymap.set('n', 'o', function()
  Dmenu.select(urls, { prompt = 'Open:' }, function(err, result)
    if err or not result then return end
    web.open(trim(result))
  end)
end)

-- Run lua code
web.keymap.set('n', 'q', function()
  Dmenu.select({}, { prompt = 'Lua >' }, function(err, result)
    if err or not result then return end
    local run, run_err = load(result)
    if run_err then print(run_err) end
    if run then run() end
  end)
end)

-- History back/forward
web.keymap.set('n', '<s-h>', function() web.history.back(); end)
web.keymap.set('n', '<s-l>', function() web.history.forward(); end)
web.keymap.set('n', '<c-w>', function() web.tabs.close(); end)

-- Dummy test keymap
web.keymap.set('n', 'm', function()
  print('Hello world. Keypress test')
end)

print('ending...')
