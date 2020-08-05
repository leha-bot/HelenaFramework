#ifndef COMMON_HFAPP_HPP
#define COMMON_HFAPP_HPP

#include <unordered_map>
#include <queue>
#include <utility>
#include <tuple>
#include <atomic>
#include <thread>

#include "HFDynLib.hpp"
#include "HFArgs.hpp"
#include "HFMeta.hpp"
#include "HFSingleton.hpp"
#include "HFXml.hpp"
#include "HFUtil.hpp"
//#include <Dependencies/entt/entt.hpp>

namespace Helena
{
	// constexpr const char* spdlog_filename(std::string_view filename) {
	// 	constexpr char symbols[]{'\\', '/'};
	// 	const auto it = std::find_first_of(filename.rbegin(), filename.rend(), std::begin(symbols), std::end(symbols));
	// 	return it == filename.rend() ? filename.data() : &(*std::prev(it));
	// }

    // // Assembly "if" optimization
	// inline decltype(auto) g_LoggerRef = spdlog::details::registry::instance();

	// #define LOG_TRACE(my_fmt, ...)		g_LoggerRef.get_default_raw()->log(spdlog::source_loc{spdlog_filename(__FILE__), __LINE__, nullptr}, spdlog::level::trace,		my_fmt, ##__VA_ARGS__);
	// #define LOG_DEBUG(my_fmt, ...)		g_LoggerRef.get_default_raw()->log(spdlog::source_loc{spdlog_filename(__FILE__), __LINE__, nullptr}, spdlog::level::debug,		my_fmt,	##__VA_ARGS__);
	// #define LOG_INFO(my_fmt, ...)		g_LoggerRef.get_default_raw()->log(spdlog::source_loc{spdlog_filename(__FILE__), __LINE__, nullptr}, spdlog::level::info,		my_fmt,	##__VA_ARGS__);
	// #define LOG_WARN(my_fmt, ...)		g_LoggerRef.get_default_raw()->log(spdlog::source_loc{spdlog_filename(__FILE__), __LINE__, nullptr}, spdlog::level::warn,		my_fmt,	##__VA_ARGS__);
	// #define LOG_ERROR(my_fmt, ...)		g_LoggerRef.get_default_raw()->log(spdlog::source_loc{spdlog_filename(__FILE__), __LINE__, nullptr}, spdlog::level::err,		my_fmt, ##__VA_ARGS__);
	// #define LOG_CRITICAL(my_fmt, ...)	g_LoggerRef.get_default_raw()->log(spdlog::source_loc{spdlog_filename(__FILE__), __LINE__, nullptr}, spdlog::level::critical,	my_fmt, ##__VA_ARGS__);    

    class HFApp final : public HFSingleton<HFApp>
    {
        friend int HelenaFramework(int, char**);

        using ModuleID = uint32_t;
        struct TypeIndexSeq final { [[nodiscard]] static ModuleID next() noexcept { static ModuleID id {}; return id++; }};

        template <typename Type, typename = void>
        struct TypeIndex {
            [[nodiscard]] static ModuleID id() noexcept { 
                static const ModuleID id = TypeIndexSeq::next();
                return id;
            }
        };

        template<typename, typename = void>
        struct has_type_index: std::false_type {};

        template<typename Type>
        struct has_type_index<Type, std::void_t<decltype(TypeIndex<Type>::id())>>: std::true_type {};

        template<typename Type>
        static inline constexpr bool has_type_index_v = has_type_index<Type>::value;

    private:
        /**
         * @brief   Initialize Helena Framework
         * 
         * @param   argc    Number of argumments
         * @param   argv    Pointer on arguments
         * 
         * @return
         * Success: @code{.cpp} true
         * @endcode
         * Failure: @code{.cpp} false
         * @endcode
         * Return false if initialize Helena Framework failure
         */
        bool Initialize(const int argc, const char* const* argv) 
        {
            std::cout << "INIT This: " << this << std::endl;
            HFArgs::Parser  argsParser("Hello, Helena!", "Good luck Helena!");
            HFArgs::Group   argsGroup(argsParser, "Helena flags:", HFArgs::Group::Validators::All);
            HFArgs::ValueFlag<std::string> argsFlag1(argsGroup, "name", "App name", {"app"});
            HFArgs::ValueFlag<std::string> argsFlag2(argsGroup, "path", "Config files path", {"config-dir"});
            HFArgs::ValueFlag<std::string> argsFlag3(argsGroup, "path", "Module files path", {"module-dir"});

            try {
                argsParser.ParseCLI(argc, argv);
            } catch (HFArgs::Help) {
                std::cerr << argsParser << std::endl;
                return true;
            } catch (HFArgs::Error& err) {
                std::cerr << argsParser << std::endl;
                std::cerr << "[Error] ArgsParse failure: " << err.what() << std::endl;
                return false;
            }

            this->m_Name = argsFlag1.Get();
            this->m_ConfigPath = argsFlag2.Get();
            this->m_ModulePath = argsFlag3.Get();

            std::cout << "[Info] App name=" << argsFlag1.Get() << std::endl;
            std::cout << "[Info] Config dir=" << argsFlag2.Get() << std::endl;
            std::cout << "[Info] Module dir=" << argsFlag3.Get() << std::endl;

            if(!this->Configuration()) {
                std::cerr << "[Error] Configuration Application failure!" << std::endl;
                return false;
            } else std::cout << "[Info] Configuration Application success!" << std::endl;

            if(!this->AppInitModule()) {
                std::cerr << "[Error] Initialize modules failure!" << std::endl;
                return false;
            } else std::cout << "[Info] Initialize modules success!" << std::endl;

            if(!this->AppInit()) {
                std::cerr << "[Error] AppInit failure!" << std::endl;
                return false;
            } else std::cout << "[Info] AppInit success!" << std::endl;

            if(!this->AppConfig()) {
                std::cerr << "[Error] AppConfig failure!" << std::endl;
                return false;
            } else std::cout << "[Info] AppConfig success!" << std::endl;

            if(!this->AppStart()) {
                std::cerr << "[Error] AppStart failure!" << std::endl;
                return false;
            } else std::cout << "[Info] AppStart success!" << std::endl;

            if(!this->AppUpdate()) {
                std::cerr << "[Error] AppUpdate failure!" << std::endl;
                return false;
            } else std::cout << "[Info] AppUpdate success!" << std::endl;

            if(!this->AppStop()) {
                std::cerr << "[Error] AppStop failure!" << std::endl;
                return false;
            } else std::cout << "[Info] AppStop success!" << std::endl;

            this->Finalize();
            return true;
        }

        void Finalize() 
        {
            for(auto& pDynLib : this->m_DynLibs) {
                pDynLib->Unload();
                HF_FREE(pDynLib);
            }
            
            this->m_DynLibs.clear();
        }
    
    public:

        /**
         * @brief   Add module in App
         * 
         * @tparam  Module  Type derived from HFModule
         * 
         * @return
         * Success: @code{.cpp} Module*
         * @endcode
         * Failure: @code{.cpp} nullptr
         * @endcode
         * Return nullptr if module already has or allocate memory failure
         */       
        template <typename Module, typename... Args, typename = std::enable_if_t<std::is_base_of_v<HFModule, Module>>>
        Module* AddModule([[maybe_unused]] Args&&... args)
        {
            const auto pDynLib = this->m_DynLibs.back();
            if(pDynLib->m_pModule) {
                std::cerr << "[Error] Module: \"" << pDynLib->GetName() << "\", error: onle one class per module!" << std::endl;
                return nullptr;               
            }

            if(const auto it = this->m_Modules.find(HF_CLASSNAME_RT(Module)); it != this->m_Modules.end()) {
                std::cerr << "[Error] Module: \"" << pDynLib->GetName() << "\", error: this class registered from other module: \"" << it->second->GetName() << "\"" << std::endl;
                return nullptr;
            }

            if(pDynLib->m_pModule = HF_NEW Module(std::forward<Args>(args)...); !pDynLib->m_pModule) {
                std::cerr << "[Error] Module: \"" << pDynLib->GetName() << "\", error: allocate memory!" << std::endl;
                return nullptr;
            }

            if(const auto [it, bRes] = this->m_Modules.emplace(HF_CLASSNAME_RT(Module), pDynLib); !bRes) {
                std::cerr << "[Error] Module: \"" << pDynLib->GetName() << "\", error: allocate memory for map!" << std::endl;
                HF_FREE(pDynLib->m_pModule);
                return nullptr;               
            }

            pDynLib->m_Version = HF_COMPILER_NAME[0];   // Take first symbol from compiler name and use how as version
            return static_cast<Module*>(pDynLib->m_pModule);
        }

        /**
         * @brief   Add module in App
         * 
         * @tparam  Module  Type derived from HFModule
         * 
         * @return
         * Success: @code{.cpp} Module*
         * @endcode
         * Failure: @code{.cpp} nullptr
         * @endcode
         * Return nullptr if module not has
         */
        template <typename Module, typename = std::enable_if_t<std::is_base_of_v<HFModule, Module>>>
        Module* GetModule() noexcept {
            const auto it = this->m_Modules.find(HF_CLASSNAME_RT(Module));
            return it == this->m_Modules.end() ? nullptr : static_cast<Module*>(it->second->m_pModule);
        }

        template <typename Module, typename... Args, typename = std::enable_if_t<std::is_base_of_v<HFModule, Module>>>
        void RemoveModule([[maybe_unused]] Args&&... args)
        {
            if(const auto it = this->m_Modules.find(HF_CLASSNAME_RT(Module)); it != this->m_Modules.end()) {
                HF_FREE(it->second->m_pModule);
                this->m_Modules.erase(it);
            }
        }

    private:

        /**
         * @brief   Initialize Helena Modules from config
         * 
         * @return
         * Success: @code{.cpp} true
         * @endcode
         * Failure: @code{.cpp} false
         * @endcode
         * Return false if same module EP not found, export function not found or callback return false
         */
        bool AppInitModule() 
        {
            for(std::string_view moduleName : this->m_ModulesConfig)
            {
                if(const auto& pDynLib = this->m_DynLibs.emplace_back(HF_NEW HFDynLib(moduleName, this)); !pDynLib->Load()) {
                    return false;
                }
            }

            if(!this->m_Modules.empty())
            {
                const auto version = this->m_Modules.begin()->second->GetVersion();
                for(const auto& data : this->m_Modules) 
                {
                    const auto module = data.second;
                    if(module->GetVersion() != version) {
                        std::cerr << "[Error] Module: \"" << module->GetName() << "\", compiler version is different!" << std::endl;
                        return false;
                    }
                }
            }

            return true;
        }

        bool AppInit() {
            
            
            return true;
        }

        bool AppConfig() {

            return true;
        }

        bool AppStart() {

            return true;
        }

        bool AppUpdate() {

            return true;
        }

        bool AppStop() {
            
            return true;
        }

        bool Configuration() 
        {
            pugi::xml_document doc;
            const std::string config(this->m_ConfigPath + HF_SEPARATOR + Meta::Helena::GetConfig().data());
            const pugi::xml_parse_result parseResult = doc.load_file(config.c_str(), pugi::parse_default | pugi::parse_comments);

            if(!parseResult) {
                std::cout << "[Error] Configuration failure, config: \"" << config << "\", error: " << parseResult.description() << std::endl;
                return false;
            }

            // Applications 
            const auto node = doc.child(Meta::Helena::GetApplications().data());
            if(node.empty()) {
                std::cout 
                << "[Error] Config: \"" << config 
                << "\", Node: \"" << Meta::Helena::GetApplications()
                << "\" not found!" << std::endl;
                return false;               
            }

            const auto node_app = node.find_child_by_attribute(Meta::Helena::GetName().data(), this->m_Name.c_str());
            if(node_app.empty()) {
                std::cout << "[Error] Config: \"" << config << "\", Node: \"" << Meta::Helena::GetApp() << "\", App Name: \"" << this->m_Name << "\" not found!" << std::endl;
                return false;
            }
            
            // Get node
            const auto node_module = node_app.child(Meta::Helena::GetModule().data());
            if(node_module.empty()) {
                std::cout 
                << "[Error] Config: \"" << config 
                << "\", App Name: \"" << this->m_Name 
                << "\", Node: \"" << Meta::Helena::GetModule() 
                << "\", node not found!" << std::endl;
                return false;                  
            }

            // Get attribute from node
            std::string_view module_name = node_module.attribute(Meta::Helena::GetName().data()).as_string(); 
            if(module_name.empty()) {
                
                std::cout 
                << "[Error] Config: \"" << config 
                << "\", App Name: \"" << this->m_Name 
                << "\", Node: \"" << Meta::Helena::GetModule() 
                << "\", module name is empty!" << std::endl;
                return false;
            }
            
            // Split attribute use symbol and trim
            this->m_ModulesConfig = HFUtil::Split<std::string>(module_name, ",", true);
            return true;
        }

    private:
        std::unordered_map<std::string, HFDynLib*> m_Modules;
        std::vector<HFDynLib*> m_DynLibs;
        std::vector<std::string> m_ModulesConfig;

        std::string m_Name;
        std::string m_ConfigPath;
        std::string m_ModulePath;
    };

    int HelenaFramework(int argc, char** argv) 
    {
        if(!HFApp::GetInstance().Initialize(argc, argv)) {
            std::cerr << "[Error] Inititalize HelenaFramework failure!" << std::endl;
            HFApp::GetInstance().Finalize();
            return 1;
        }
        
        return 0;
    }
}

#endif