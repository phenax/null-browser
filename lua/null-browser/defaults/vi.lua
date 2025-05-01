local M = {}

local config = {
  menu = require 'null-browser.extras.dmenu',
  history = require 'null-browser.extras.history',
  transform_url_input = web.utils.string_trim,
}

function M.setup(opts)
  config = web.utils.table_merge(config, opts)
end

function M.initialize()
  -- Start in normal mode
  web.keymap.set_mode('n')
  web.keymap.set('n', 'i', function() web.keymap.set_mode('i') end)
  web.keymap.set('i', '<esc>', function() web.keymap.set_mode('n') end)

  -- Scrolling
  web.keymap.set('n', 'j', function() web.view.scroll(0, 20) end)
  web.keymap.set('n', 'k', function() web.view.scroll(0, -20) end)
  web.keymap.set('n', '<s-j>', function() web.view.scroll(0, 80) end)
  web.keymap.set('n', '<s-k>', function() web.view.scroll(0, -80) end)
  web.keymap.set('n', 'gg', function() web.view.scroll_to_top() end)
  web.keymap.set('n', '<s-g>', function() web.view.scroll_to_bottom() end)

  -- Open in new view
  web.keymap.set('n', 'o', function()
    config.menu.select(config.history.list(), { prompt = 'Open view:' }, function(err, result)
      if err or not result then return end
      web.view.create(config.transform_url_input(result))
    end)
  end)
  -- Open in current view
  web.keymap.set('n', '<s-o>', function()
    config.menu.select(config.history.list(), { prompt = 'Open:' }, function(err, result)
      if err or not result then return end
      web.view.set_url(config.transform_url_input(result))
    end)
  end)
  -- Delete from history
  web.keymap.set('n', 'dh', function()
    config.menu.select(config.history.list(), { prompt = 'Delete history:' }, function(err, result)
      if err or not result then return end
      config.history.delete(web.utils.string_trim(result))
    end)
  end)

  -- Search
  web.keymap.set('n', '<c-f>', function()
    config.menu.input({ prompt = 'Search:', input = web.search.get_text() }, function(err, input)
      if err then return end
      web.search.set_text(input)
    end)
  end)
  web.keymap.set('n', '<esc>', function() web.search.reset() end)
  web.keymap.set('n', 'n', function() web.search.next() end)
  web.keymap.set('n', '<s-n>', function() web.search.previous() end)

  -- Run lua code
  web.keymap.set('n', 'q', function()
    config.menu.input({ prompt = 'Lua:' }, function(err, result)
      if err or not result then return end
      local run, run_err = load(result)
      if run_err then print(run_err) end
      if run then run() end
    end)
  end)

  -- History back/forward
  web.keymap.set('n', '<s-h>', function() web.history.back(); end)
  web.keymap.set('n', '<s-l>', function() web.history.forward(); end)

  -- Close view
  web.keymap.set('n', '<c-w>', function() web.view.close(); end)
  web.keymap.set('n', 'dv', function() web.view.close(); end)

  -- View select
  web.keymap.set('n', 'b', function()
    local views_list = {}
    local views = web.view.list()
    for index, view in ipairs(web.view.list()) do
      table.insert(views_list, index .. ': ' .. view.title .. ' (' .. view.url .. ')')
    end

    config.menu.select(views_list, { prompt = 'Views:' }, function(err, result)
      if err or not result then return end
      local index_str, _ = string.gsub(result, '%s*:.*$', '')
      local index = tonumber(index_str)
      if views[index] then
        web.view.select(views[index].id)
      end
    end)
  end)

  -- 1-0 for view indexes 1-10
  for index = 1, 10 do
    local key = index
    if index >= 10 then key = 0 end
    web.keymap.set('n', '<space>' .. key, function()
      local views = web.view.list()
      if index > #views then return end
      web.view.select(views[index].id)
    end)
  end

  -- Next view
  web.keymap.set('n', 'bn', function()
    local views = web.view.list()
    if #views <= 1 then return end
    local index = web.view.current_index() + 1;
    if index > #views then index = 1 end
    web.view.select(views[index].id)
  end)

  -- Prev view
  web.keymap.set('n', 'bp', function()
    local views = web.views.list()
    if #views <= 1 then return end
    local index = web.view.current_index() - 1;
    if index <= 0 then index = #views end
    web.views.select(views[index].id)
  end)

  -- Update current url
  web.keymap.set('n', '<c-l>', function()
    local views = web.view.list()
    local view = views[web.view.current_index()];
    if view == nil then return end
    config.menu.select(config.history.list(), { prompt = 'Set url:', input = view.url }, function(err, result)
      if err or not result then return end
      web.view.set_url(web.utils.string_trim(result))
    end)
  end)

  -- Open devtools
  web.keymap.set('n', '<c-i>', function() web.view.open_devtools() end)
end

M.initialize()

return M
