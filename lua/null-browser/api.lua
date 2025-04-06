print("api load started")

--- @type table
web = web

local function shallow_copy(t)
  local t2 = {}
  for k, v in pairs(t) do t2[k] = v end
  return t2
end

-- inspect lua tables
local inspector_loaded, inspector = pcall(require, 'inspect')
if inspector_loaded then
  web.inspect = inspector.inspect
else
  web.inspect = function() print('[warn] "inspect" module not loaded') end
end

-- web.keymap = web.keymap or {}
-- web.tab = web.tab or {}
web.event = web.event or {}

--- Listen to events from the browser
---
--- @class null.EventListenerOptions
--- @field callback any  Callback called when that event occurs
---
--- @param events string|table             Event or events to listen to
--- @param opts null.EventListenerOptions  Options
---
--- @example
--- ```lua
--- web.event.add_listener('UrlChanged', {
---   callback = function(opts)
---     print(opts.url, opts.tab, opts.win)
---   end,
--- })
--- ```
function web.event.add_listener(events, opts)
  opts = shallow_copy(opts or {})
  if type(events) == "string" then events = { events } end
  opts.events = events or {}

  return __internals.register_event(opts)
end

print("api loaded")
