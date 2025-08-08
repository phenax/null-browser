local function get_labels(items)
  local labels = {}
  for _, item in ipairs(items) do
    if item.items then
      for _, label in ipairs(get_labels(item.items)) do
        table.insert(labels, label)
      end
    else
      local label = table.concat(item.names_hierarchy, '.')
      table.insert(labels, label)
    end
  end
  return labels
end

local function get_dir()
  local dir = './doc'
  for index, a in ipairs(arg) do
    if a == '--dir' or a == '-d' then
      dir = arg[index + 1]
    end
  end
  return dir
end

return {
  lua = function(doc)
    local labels = get_labels(doc)
    local file_path = get_dir() .. '/symbols.json'
    local file, _ = io.open(file_path, 'w+')
    if file then
      local symbols = require 'dkjson'.encode(labels, { indent = true })
      assert(file:write(symbols))
    end
    return false
  end,
}
