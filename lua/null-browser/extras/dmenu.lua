local Dmenu = {}

--- @param opts? table
function Dmenu:new(opts)
  opts = opts or {}
  local default_opts = {
    command = 'dmenu',
    args = {},
    prompt_arg = '-p',
    query_arg = '-it',
    prompt = nil,
    query = nil,
  }
  local obj = {}
  setmetatable(obj, self)
  self.__index = self
  obj.options = web.utils.table_merge(default_opts, opts or {})
  return obj
end

function Dmenu:prepare_command(opts)
  opts = opts or {}
  local args = web.utils.table_merge({}, self.options.args or {})
  for _, arg in ipairs(opts.args or {}) do
    table.insert(args, arg)
  end

  local options = web.utils.table_merge({}, self.options, opts or {})

  if options.prompt then
    table.insert(args, options.prompt_arg)
    table.insert(args, options.prompt)
  end

  if options.query then
    table.insert(args, options.query_arg)
    table.insert(args, options.query)
  end

  return { command = options.command, args = args }
end

function Dmenu:select(list, opts, callback)
  local cmd = self:prepare_command(opts)

  local stdin = web.uv.new_pipe();
  local stdout = web.uv.new_pipe();

  local selection = nil
  web.uv.spawn(cmd.command, { args = cmd.args, stdio = { stdin, stdout, nil } }, function(code)
    web.uv.close(stdout)
    web.uv.close(stdin)

    if code == 0 then
      callback(nil, selection)
    else
      callback('[dmenu] Exit with status code: ' .. code, selection)
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

function Dmenu:input(opts, callback)
  return self:select({}, opts, callback)
end

return Dmenu
