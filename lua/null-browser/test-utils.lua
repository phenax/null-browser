local luassert = require 'luassert'
local match = require 'luassert.match'
local spy = require 'luassert.spy'
local stub = require 'luassert.stub'

local t = {
  before_each_hooks = {},
}

local level = 0

local function with_indent(func, ...)
  level = level + 1
  func(...)
  t.before_each_hooks[level] = nil
  level = level - 1
end

function t.before_each(callback)
  t.before_each_hooks[level] = callback
end

function t.describe(description, func)
  local prefix = string.rep(' ', level)
  print(prefix .. '\x1b[1m' .. description .. '\x1b[0m')
  io.flush()

  with_indent(func)
end

function t.context(description, func)
  local prefix = string.rep(' ', level)
  print(prefix .. '├ \x1b[32m' .. description .. '\x1b[0m')
  io.flush()

  with_indent(func)
end

function t.it(description, func)
  local prefix = string.rep(' ', level - 1)
  print(prefix .. '└─⚪\x1b[36m' .. description .. '\x1b[0m')
  io.flush()

  local snapshot = assert:snapshot()
  for _, value in pairs(t.before_each_hooks) do
    if value and type(value) == 'function' then
      value()
    end
  end
  with_indent(func)
  snapshot:revert()
end

---@diagnostic disable-next-line: unused-local
function t.xit(description, func)
  local prefix = string.rep(' ', level - 1)
  print(prefix .. '└─\x1b[33m⚠ SKIPPED ' .. description .. '\x1b[0m')
  io.flush()
end

_G.describe = t.describe
_G.context = t.context
_G.it = t.it
_G.xit = t.xit
_G.assert = luassert
_G.spy = spy
_G.match = match
_G.stub = stub

local function deep_match(matcher, value)
  if matcher == value then return true end

  for key, match_val in pairs(matcher) do
    if match_val ~= value[key] then
      if match.is_matcher(match_val) then
        if not match_val(value[key]) then return false end
      elseif type(match_val) == 'table' and not deep_match(match_val, value[key]) then
        return false
      elseif not web.utils.equals(match_val, value[key]) then
        return false
      end
    end
  end

  return true
end

assert:register('matcher', 'shape', function(_, arguments)
  return function(value) return deep_match(arguments[1], value) end
end)

return t
