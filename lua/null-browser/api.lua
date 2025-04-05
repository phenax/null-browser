print("api loaded")

local function shallow_copy(t)
  local t2 = {}
  for k, v in pairs(t) do t2[k] = v end
  return t2
end

-- web.keymap = web.keymap or {}
-- web.tab = web.tab or {}
web.event = web.event or {}

web.event.add_listener = function(events, opts)
  opts = shallow_copy(opts or {})
  if type(events) ~= "table" then events = { events } end
  opts.events = events or {}

  __internals.register_event(opts)
end
