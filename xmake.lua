add_rules("mode.debug", "mode.release")

local runtime = "MT"

if is_mode("debug") then
    add_defines("_DEBUG")
    set_targetdir("Build/Debug")

    set_runtimes(runtime .. "d")
    set_symbols("debug", "edit")
else
    add_undefines("_DEBUG")
    set_targetdir("Build/Release")

    set_runtimes(runtime)
end

target("Launcher")
    set_languages("c++latest")
    set_kind("binary")
    add_files("Source/**.cpp")

    add_cxflags("/utf-8", "/Zc:wchar_t")

    add_links(
        "kernel32", "user32", "shell32", "ntdll"
    )