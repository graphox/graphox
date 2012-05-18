--[[!
    Variable: writers
    An array containing a key - value list of available writers.
    
    nil - returns "nil"
    number - returns tonumber(number)
    string - returns tostring(string)
    boolean - returns "true" or "false"
    table - returns the table as a string
    function - returns the function
]]

local writers

--[[!
	Function: serialize_data
	Executes the writer from the writers table wich suites the type
	
	Parameters
		data - the data to write
		level - the number of tabs for outline
	
	Return:
		the result of the writer
]]

function serialize_data (data, level)
	return writers[type(data)](data, level)
end

--[[!
	Function: writetabs
	Writes a specified number of tabs
	
	Parameters
		level - number of tabs
	
	Return:
		a string containing the tabs
]]

function writetabs (level)
	local string = ""
	
	for i = 1, level do
		string = string .. "\t"
	end
	
	return string
end

writers = {
	["nil"] = function (item)
		return "nil"
	end,
	
	["number"] = function (item)
		return tostring(item)
	end,
	
	["string"] = function (item)
		return string.format("%q", item)
	end,
	
	["boolean"] = function (item)
		if item then
			return "true"
		else
			return "false"
		end
	end,
	
	["table"] = function (item, level)
		local string = ""
		
		string = string .. "{\n"
		
		for k, v in pairs(item) do
			string = string .. writetabs(level+1)
			string = string .. "["
			string = string .. serialize_data(k, level + 1)
			string = string .. "] = "
			
			string = string	.. serialize_data(v, level + 1)
			
			string = string .. ",\n"
		end
		
		string = string .. writetabs(level) .. "}"

		return string
	end,

	--Works only on lua functions without upvalues!	
	["function"] = function (item)
		local dInfo = debug.getinfo(item, "uS");
		if dInfo.nups > 0 then
			return "nil --[[ upvalues not supported ]]"
		elseif dInfo.what ~= "Lua" then
			return "nil --[[ non-lua function not supported ]]"
		else
			local r, s = pcall(string.dump,item);
			
			if r then
				return "loadstring(%(1)q)" % {s}
			else
				return "nil --[[function could not be dumped]]"
			end
		end
	end,
	
	["thread"] = function (item)
		return "nil --[[thread]]\n";
	end,
	
	["userdata"] = function (item)
		return "nil --[[userdata]]\n"
	end
}
