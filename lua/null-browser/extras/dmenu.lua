--- @type table
uv = uv

local dmenu = {}

function dmenu.select(list, opts, callback)
  local selection = nil
  local stdin = uv.new_pipe();
  local stdout = uv.new_pipe();
  local args = { '-p', opts.prompt or '>', '-it', opts.input or '' }
  uv.spawn('dmenu', { args = args, stdio = { stdin, stdout, nil } }, function(code)
    uv.close(stdout)
    uv.close(stdin)
    if code == 0 then
      callback(nil, selection)
    else
      callback('Exit with status code: ' .. code, selection)
    end
  end)

  uv.read_start(stdout, function(_, data)
    if data then selection = data end
  end)

  for _, value in ipairs(list) do
    uv.write(stdin, value .. '\n')
  end
  uv.shutdown(stdin)
end

function dmenu.input(opts, callback)
  dmenu.select({}, opts, callback)
end

return dmenu
