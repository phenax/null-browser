--- @type table
_G.web = _G.web or {}
web.utils = web.utils or {}

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
