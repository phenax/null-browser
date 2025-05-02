local Dmenu = require 'null-browser.extras.dmenu'
local t = require 'null-browser.test-utils'

describe('class: extras.dmenu', function()
  t.before_each(function()
    stub(web.uv, 'spawn')
    stub(web.uv, 'new_pipe', 0)
    stub(web.uv, 'close')
    stub(web.uv, 'read_start')
    stub(web.uv, 'shutdown')
    stub(web.uv, 'write')
  end)

  describe('#select', function()
    context('with default options', function()
      it('spawns the dmenu process', function()
        local dmenu = Dmenu:new {}
        local spawn_proccess_stub = stub(web.uv, 'spawn')

        dmenu:select({}, {}, function() end)

        assert.stub(spawn_proccess_stub).was.called_with(
          'dmenu',
          match.shape({ args = {}, stdio = match.table() }),
          match.is_function()
        )
      end)
    end)

    context('with custom command options', function()
      it('spawns the custom dmenu process', function()
        local dmenu = Dmenu:new { command = 'some-other-dmenu', args = { 'my-arg' } }
        local spawn_proccess_stub = stub(web.uv, 'spawn')

        dmenu:select({}, {}, function() end)

        assert.stub(spawn_proccess_stub).was.called_with(
          'some-other-dmenu',
          match.shape({ args = { 'my-arg' }, stdio = match.table() }),
          match.is_function()
        )
      end)
    end)

    context('when a list is provided', function()
      it('writes the list to stdin of process', function()
        local dmenu = Dmenu:new {}
        local write_data_spy = stub(web.uv, 'write')

        dmenu:select({ 'hello', 'world', 'testing' }, {}, function() end)

        assert.stub(write_data_spy).was.called_with(0, 'hello\n')
        assert.stub(write_data_spy).was.called_with(0, 'world\n')
        assert.stub(write_data_spy).was.called_with(0, 'testing\n')
      end)
    end)

    context('when process exits with status code 0', function()
      context('when process outputs to stdout', function()
        it('calls result callback with output line and no error', function()
          local dmenu = Dmenu:new {}
          local invoke_process_exit = nil
          local on_result = spy.new()
          stub(web.uv, 'spawn').invokes(function(_, _, callback)
            invoke_process_exit = callback
          end)
          stub(web.uv, 'read_start').invokes(function(_, callback)
            callback(nil, 'user input')
            if invoke_process_exit then invoke_process_exit(0) end
          end)

          dmenu:select({}, {}, on_result)

          assert.stub(on_result).was.called_with(nil, 'user input')
        end)
      end)

      context('when process does not output to stdout', function()
        it('calls result callback with nil and no error', function()
          local dmenu = Dmenu:new {}
          local on_result = spy.new()
          stub(web.uv, 'spawn').invokes(function(_, _, callback)
            callback(0)
          end)

          dmenu:select({}, {}, on_result)

          assert.stub(on_result).was.called_with(nil, nil)
        end)
      end)
    end)

    context('when process exits with non-zero status code', function()
      context('when process outputs to stdout', function()
        it('calls result callback with an error and output line', function()
          local dmenu = Dmenu:new {}
          local invoke_process_exit = nil
          local on_result = spy.new()
          stub(web.uv, 'spawn').invokes(function(_, _, callback)
            invoke_process_exit = callback
          end)
          stub(web.uv, 'read_start').invokes(function(_, callback)
            callback(nil, 'user input')
            if invoke_process_exit then invoke_process_exit(42) end
          end)

          dmenu:select({}, {}, on_result)

          assert.stub(on_result).was.called_with(match.string(), 'user input')
        end)
      end)

      context('when process does not output to stdout', function()
        it('calls result callback with an error and nil', function()
          local dmenu = Dmenu:new {}
          local on_result = spy.new()
          stub(web.uv, 'spawn').invokes(function(_, _, callback)
            callback(42)
          end)

          dmenu:select({}, {}, on_result)

          assert.stub(on_result).was.called_with(match.string(), nil)
        end)
      end)
    end)
  end)
end)
