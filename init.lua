dofile "script/serialize.lua"

package.path = package.path .. ";script/package/?.lua;"
package.cpath = package.cpath .. ";lib/lib?.so"
package.cpath = package.cpath .. ";lib/l?.so"

require "ldbus"

local connection = ldbus.bus ('session')

ldbus.iterate()
connection:call({
	bus = "org.bansheeproject.Banshee",
	path = "/org/bansheeproject/Banshee/PlayerEngine",
	interface = "org.bansheeproject.Banshee.PlayerEngine",
	method = 'GetProperties',	
}, function(msg) print(msg); print(serialize_data(msg)) end)
ldbus.iterate()

print(vars.version())

i = 0
event = {
	frame = function()
		i = i + 1
		if i == 200 then
			i = 0
			ldbus.iterate()
			print("dbus!")
			
		end
	end,
	
	connect = function(cn)
		print(string.format("CONNECT: %i", cn or -1))
	end,
}
