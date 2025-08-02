local hints = {
  config = {
    mode = 'f',
  },
}

local js_setup_code = ''

function hints.init(on_ready)
  hints.load_finder_js(function()
    web.keymap.set(hints.config.mode, '<Esc>', function()
      hints.stop()
    end)

    web.event.add_listener('KeyPressed', {
      callback = function(event)
        if web.keymap.get_mode() == hints.config.mode then
          hints.filter_key(event.key)
        end
      end,
    })

    if type(on_ready) == 'function' then on_ready() end
  end)
end

function hints.start(selector, new_view)
  local open_in_new_view = new_view and 'true' or 'false'
  web.view.run_js(
    js_setup_code ..
    ";_nullbrowser.finder.start('" ..
    selector ..
    "', " ..
    open_in_new_view ..
    ")"
  )
  web.schedule(function()
    -- Trigger f mode after tick to avoid the trigger key (f) to get captured in event
    web.keymap.set_mode(hints.config.mode)
  end)
end

function hints.filter_key(key)
  web.view.run_js("_nullbrowser.finder.filterOutByKey('" .. key .. "')")
end

function hints.stop()
  web.keymap.set_mode('n')
  web.view.run_js('_nullbrowser.finder.stop()')
end

function hints.load_finder_js(on_ready)
  web.uv.fs_open('./assets/javascript/finder.js', 'r', 438, function(err, file)
    if err then return end
    if not file then return {} end
    local stat = assert(web.uv.fs_fstat(file))
    local js = assert(web.uv.fs_read(file, stat.size))
    assert(web.uv.fs_close(file))
    js_setup_code = js

    on_ready()
  end)
end

return hints
