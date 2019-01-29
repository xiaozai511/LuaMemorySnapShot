local StrMemorySnapPath = "memory"
local StrScriptRequirePath = "memory/MemoryReferenceInfo"
local StrScriptRequirePath = "MemoryReferenceInfo"

-- --创建目录
-- local function _createDirectory(path)
-- 	local filename = path .. "/tmp.txt"
-- 	local logAgent = Captain.LogCoreAgent.sharedCoreAgent()
-- 	local logHandle = logAgent:RegisterCatagory(filename)
-- 	logAgent:CloseCatagory(logHandle)
-- 	logAgent:DeleteCatagory(filename)
-- end

-- --创建文件
-- local function _createFileHandler(filename)
-- 	local logAgent = Captain.LogCoreAgent.sharedCoreAgent()
-- 	local logHandle = logAgent:RegisterCatagory(filename)
-- 	-- logAgent:CloseCatagory(logHandle)
-- end

-- local function _createOutputEntry(filename)
-- 	local logAgent = Captain.LogCoreAgent.sharedCoreAgent()
-- 	local logHandle = logAgent:RegisterCatagory(filename)
-- 	assert(logAgent and logHandle)
-- 	local cOutputHandle = logHandle
-- 	local cOutputEntry = function(handle, str)
-- 						logAgent:LogString(handle, str)
-- 					end
-- 	return cOutputEntry, cOutputHandle
-- end

-- local function _closeFileHandler(logHandle)
-- 	local logAgent = Captain.LogCoreAgent.sharedCoreAgent()
-- 	logAgent:CloseCatagory(logHandle)
-- end

-- local function _readFile(filename)
-- 	local logAgent = Captain.LogCoreAgent.sharedCoreAgent()
-- 	local logHandle = logAgent:RegisterCatagory(filename)
	
-- 	logAgent:CloseCatagory(logHandle)
-- end

--snap shot
local function MemorySnapshot(parames)
	local mode = tostring(parames[1])
	local mri = require(StrScriptRequirePath)
	local path = parames[2] or StrMemorySnapPath
	if mode == "A" then
		--打印lua虚拟机的内存快照
		collectgarbage("collect")
		mri.m_cMethods.DumpMemorySnapshot(path, "all", -1)
	elseif mode == "S" then
		--打印对象内存快照
		collectgarbage("collect")
		mri.m_cMethods.DumpMemorySnapshotSingleObject(path, "SingleObj", -1, "_G", _G)
	elseif mode == "C" then
		--对比两份内存快照
		local src_before = tostring(parames[3])
		local src_after = tostring(parames[4])
		mri.m_cMethods.DumpMemorySnapshotComparedFile(path, "Compared", -1, src_before, src_after)
	elseif mode == "F" then	--暂时没用到
		--从内存快照过滤关键字
		local strFilePath = tostring(parames[3])
		local fiter_name = tostring(parames[4])
		mri.m_cBases.OutputFilteredResult(path.."/"..strFilePath, fiter_name, true, true)
	elseif mode == "~F" then	--暂时没用到
		--从内存快照过滤关键字
		local strFilePath = tostring(parames[3])
		local fiter_name = tostring(parames[4])
		mri.m_cBases.OutputFilteredResult(path.."/"..strFilePath, fiter_name, false, true)
	end
	-- collectgarbage("collect")
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function DoMemorySnapShot( ... )
	MemorySnapshot({...})
end

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

local function Main(args)
	if #args < 1 then
		return
	elseif args[1] == "help" then
		print([[
			Useage: MemorySnapshot [OPTION] [ARGS]
				OPTION:
				A  --- new shot, args = [output_path]
				C  --- compare, args = [output_path] [file_befor] [file_after]
				F  --- filter, args = [file_path] [filter_name]
				~F --- not filter, args = [file_path] [filter_name]
			]])
		return
	end

	-- MemorySnapshot(args)
	-- for i, str in ipairs(args) do
	-- 	print(i, str)
	-- end
	DoMemorySnapShot(unpack(args))
end

Main(arg)
