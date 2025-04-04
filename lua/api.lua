local function shallow_copy(t)
  local t2 = {}
  for k, v in pairs(t) do t2[k] = v end
  return t2
end

web.event.add_listener = function(events, opts)
  opts = shallow_copy(opts or {})
  if type(events) ~= "table" then events = { events } end
  opts.events = events

  __internals.register_event(opts)
end
