local M = {
  urls = {
    g = 'https://google.com/search?q={}',
    d = 'https://duckduckgo.com/?q={}',
  },
  default = 'd',
}

function M.preprocess_url(url)
  local url_trimmed, _ = string.gsub(url, '^%s*(.-)%s*$', '%1')
  if M.is_url(url_trimmed) then return url_trimmed end

  for key, _ in pairs(M.urls) do
    local url_out, found = M.parse_search_string(key, url_trimmed)
    if found then return M.substitute(key, url_out) end
  end

  return M.substitute(M.default, url_trimmed)
end

function M.is_url(str) return string.match(str, "^https?://") end

function M.substitute(search_name, value)
  local res, _ = string.gsub(M.urls[search_name], '{}', value)
  return res
end

function M.parse_search_string(search_name, value)
  local res, count = string.gsub(value, '^%s*' .. search_name .. '%s+', '')
  return res, count > 0
end

return M
