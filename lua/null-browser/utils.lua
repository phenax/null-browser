--- Helper lua utils for null browser
--- @module web

_G.web = _G.web or {}
web.utils = web.utils or {}

--- luv api (https://github.com/luvit/luv/blob/master/docs.md)
---
--- @diagnostic disable-next-line: undefined-global
web.uv = web.uv or nil

local inspector = require 'null-browser.inspect'

--- Returns human-readable string representation of Lua tables (https://github.com/kikito/inspect.lua)
--- @diagnostic disable-next-line: undefined-doc-param
--- @param value any
--- @return string
web.inspect = inspector.inspect

--- Print a lua object to inspect
--- @param value any
function web.print(value)
  print(web.inspect(value))
end

web.json = web.json or {}
local json = require 'null-browser.json'

--- Encodes lua object as json into a string
--- @param obj any                Value to encode into json string
--- @param opts table?            Options
--- @param opts.indent boolean?   Whether to enable indentation (default: false)
--- @return string
web.json.encode = function(obj, opts)
  opts = opts or {}
  local state = { indent = opts.indent or false }
  return tostring(json.encode(obj, state))
end

--- Decodes a string into a lua object
--- @diagnostic disable-next-line: undefined-doc-param
--- @param json_str string        Json string to decode
--- @param opts table             Options
--- @param opts.null any          What to use as null value (default: nil)
--- @return any
web.json.decode = function(json_str, opts)
  opts = opts or {}
  local value, _, err = json.decode(json_str, nil, opts.null)
  if err then error(err) end
  return value
end

--- Trim whitespace from start and end of a string
--- @param str string
--- @return string
function web.utils.string_trim(str)
  local res, _ = string.gsub(str, '^%s*(.-)%s*$', '%1')
  return res
end

--- Merge multiple tables
--- @param tbl table
--- @return table
function web.utils.table_merge(tbl, ...)
  for i = 1, select("#", ...) do
    for k, v in pairs(select(i, ...) or {}) do
      tbl[k] = v
    end
  end

  return tbl
end

--- Get keys of a table
--- @param tbl table
--- @return table
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

--- Check if 2 values are deeply equal
--- @param a any
--- @param b any
--- @return boolean
function web.utils.equals(a, b)
  return is_deep_equal_helper(a, b, {})
end

--- Check if table contains given value
--- @param tbl table
--- @param value any
--- @return boolean
function web.utils.table_contains(tbl, value)
  for _, tbl_val in pairs(tbl) do
    if tbl_val == value then
      return true
    end
  end
  return false
end
