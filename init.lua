dofile "script/serialize.lua"

print(vars.version())

event = {
	frame = function()

	end,
	
	connect = function(cn)
		print(string.format("CONNECT: %i", cn or -1))
	end,
}
