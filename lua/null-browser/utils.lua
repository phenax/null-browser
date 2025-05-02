--- @type table
_G.web = _G.web or {}
web.utils = web.utils or {}

--- luv api
--- @type table
--- @link https://github.com/luvit/luv/blob/master/docs.md
--- @diagnostic disable-next-line: undefined-global
web.uv = web.uv or nil

local inspector = require 'null-browser.inspect'

--- Returns human-readable string representation of Lua tables
---
--- @link https://github.com/kikito/inspect.lua
web.inspect = inspector.inspect

-- TODO: Documentation
function web.utils.string_trim(str)
  local res, _ = string.gsub(str, '^%s*(.-)%s*$', '%1')
  return res
end

-- TODO: Documentation
function web.utils.table_merge(t, ...)
  for i = 1, select("#", ...) do
    for k, v in pairs(select(i, ...)) do
      t[k] = v
    end
  end

  return t
end

function web.utils.table_keys(tbl)
  local keys = {}
  for key, _ in pairs(tbl) do
    table.insert(keys, key)
  end
  return keys
end

local function is_deep_equal_helper(a, b, table_pairs)
  if type(a) ~= type(b) then return false end
  if type(a) == 'table' and type(b) == 'table' then
    local keys_a = web.utils.table_keys(a)
    local keys_b = web.utils.table_keys(b)
    if #keys_a ~= #keys_b then return false end

    if table_pairs[a] == b then
      return true
    end

    table_pairs[a] = b

    for k in pairs(a) do
      if not is_deep_equal_helper(a[k], b[k], table_pairs) then return false end
    end

    return true
  end

  return a == b
end

function web.utils.equals(a, b)
  return is_deep_equal_helper(a, b, {})
end
