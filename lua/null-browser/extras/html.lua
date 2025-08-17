local html = {}

function html.create_el(name)
  return function(attrs, children)
    return { name = name, attrs = attrs, children = children }
  end
end

function html.el(name, attrs, children)
  return html.create_el(name)(attrs, children)
end

function html.to_string(elem)
  if type(elem) == "string" then return html.escape_string(elem) end

  if elem.raw and elem.value then return elem.value end

  local attrs_str = ''
  for key, value in pairs(elem.attrs or {}) do
    -- TODO: html escape + quotify
    attrs_str = attrs_str .. ' ' .. key .. '="' .. html.escape_string(value) .. '"'
  end

  local children_str = ''
  for _, value in ipairs(elem.children or {}) do
    children_str = children_str .. ' ' .. html.to_string(value)
  end

  return '<' .. elem.name .. attrs_str .. '>' .. children_str .. '</' .. elem.name .. '>'
end

function html.raw(value)
  return { raw = true, value = value };
end

html.div = html.create_el('div')
html.span = html.create_el('span')
html.button = html.create_el('button')
html.style = html.create_el('style')
html.a = html.create_el('a')
html.input = html.create_el('input')

function html.escape_string(str)
  return str:gsub('&', '&amp;')
      :gsub('<', '&lt;')
      :gsub('>', '&gt;')
      :gsub('"', '&quot;')
      :gsub("'", '&apos;')
end

return html
