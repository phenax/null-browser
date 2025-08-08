local M = {
  urls = {
    g = 'https://google.com/search?q={}',
    d = 'https://duckduckgo.com/?q={}',
  },
  default = 'd',
}

function M.transform_url_input(input)
  local url = web.utils.string_trim(input)
  if M.is_url(url) then return url end

  for key, _ in pairs(M.urls) do
    local url_out, found = M.parse_search_string(key, url)
    if found then return M.substitute(key, url_out) end
  end

  return M.substitute(M.default, url)
end

function M.is_url(str)
  local protocol = string.gsub(str, '://.*$', '')
  local patterns = { 'https', 'http', 'file', 'null' }
  return web.utils.table_contains(patterns, protocol)
end

function M.substitute(search_name, value)
  local res, _ = string.gsub(M.urls[search_name], '{}', value)
  return res
end

function M.parse_search_string(search_name, value)
  local res, count = string.gsub(value, '^%s*' .. search_name .. '%s+', '')
  return res, count > 0
end

return M
