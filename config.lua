print('hello starting up...')

--- @type table
web = web
--- @type table
uv = uv

web.keymap.set('n', 'm', function()
  print('Hello world. Keypress test')
end)

web.keymap.set('n', 'o', function()
  web.tabopen('https://lite.duckduckgo.com')
end)

print('ending...')
