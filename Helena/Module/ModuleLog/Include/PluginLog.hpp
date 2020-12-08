#ifndef MODULELOG_PLUGINLOG_HPP
#define MODULELOG_PLUGINLOG_HPP

#include <Interface/IPluginLog.hpp>

namespace Helena
{
	class PluginLog : public IPluginLog 
	{
	protected:

	public:
		PluginLog() = default;
		~PluginLog();

	private:
		void Configure();
		std::string GetFileLog(const std::string_view path);
		void SetupLoggerST(const std::string_view path);
		void SetupLoggerMT(const std::string_view path, const std::size_t buffer, const std::size_t threads);
		std::shared_ptr<spdlog::logger> GetLogger() override;

	private:
		std::shared_ptr<spdlog::logger> m_pLogger;
		std::shared_ptr<spdlog::details::thread_pool> m_pThreadPool;
	};
}

#endif // MODULELOG_PLUGINLOG_HPP