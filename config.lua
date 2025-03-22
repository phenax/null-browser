print('hello starting up...')

--- @type table
web = web
--- @type table
uv = uv

local Dmenu = {}
function Dmenu.select(list, callback)
  local selection = nil
  local stdin = uv.new_pipe();
  local stdout = uv.new_pipe();
  uv.spawn('dmenu', { stdio = { stdin, stdout, nil } }, function(code)
    print('exit')
    uv.close(stdout)
    print('exit close out')
    uv.close(stdin)
    print('exit close in')
    if code == 0 then
      callback(nil, selection)
    else
      callback('Exit with status code: ' .. code, selection)
    end
    print('done exit')
  end)

  uv.read_start(stdout, function(err, data)
    print('-> out', err, data)
    if err ~= nil then
      callback(err, nil)
    elseif data then
      selection = data
    end
  end)

  for _, value in ipairs(list) do
    print('input write')
    uv.write(stdin, value .. '\n')
  end
  print('shit')
  uv.shutdown(stdin)
  print('end')
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
  Dmenu.select(urls, function(err, result)
    if err or not result then return end
    web.tabopen(trim(result))
  end)
end)
-- Open in current tab
web.keymap.set('n', 'o', function()
  Dmenu.select(urls, function(err, result)
    if err or not result then return end
    web.open(trim(result))
  end)
end)
-- Run lua code
web.keymap.set('n', 'q', function()
  Dmenu.select({}, function(err, result)
    if err or not result then return end
    local run, run_err = load(result)
    if run_err then print(run_err) end
    if run then run() end
  end)
end)

-- Dummy test keymap
web.keymap.set('n', 'm', function()
  print('Hello world. Keypress test')
end)

print('ending...')
