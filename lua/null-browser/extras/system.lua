local system = {}

function system.copy_to_clipboard(text)
  local pipe = io.popen('xclip -selection clipboard', 'w')
  if not pipe then return end
  pipe:write(text)
  pipe:close()
end

return system
