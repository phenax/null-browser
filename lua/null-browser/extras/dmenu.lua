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
    select_last_line = true,
    transform_output = nil,
  }
  local obj = {}
  setmetatable(obj, self)
  self.__index = self
  obj.options = web.utils.table_merge(default_opts, opts or {})
  return obj
end

function Dmenu:prepare_options(opts)
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

  return {
    command = options.command,
    args = args,
    select_last_line = options.select_last_line,
    transform_output = options.transform_output or function(s) return s end,
  }
end

function Dmenu:select(list, opts, callback)
  local options = self:prepare_options(opts)

  local stdin = web.uv.new_pipe();
  local stdout = web.uv.new_pipe();

  local selection = nil
  if not options.select_last_line then
    selection = {}
  end
  web.uv.spawn(options.command, { args = options.args, stdio = { stdin, stdout, nil } }, function(code)
    web.uv.close(stdout)
    web.uv.close(stdin)

    local result = options.transform_output(selection)
    if code == 0 then
      callback(nil, result)
    else
      callback('[dmenu] Exit with status code: ' .. code, result)
    end
  end)

  web.uv.read_start(stdout, function(_, data)
    if data then
      if options.select_last_line then
        selection = data
      else
        selection = selection or {}
        table.insert(selection, data)
      end
    end
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
