#include "compilable.hpp"

namespace bacs{namespace system{namespace builders
{
    class interpretable: public compilable
    {
    protected:
        name_type name(const std::string &source) override;
    };

    class interpretable_executable: public compilable_executable
    {
    public:
        interpretable_executable(
            const ContainerPointer &container,
            bunsan::tempfile &&tmpdir,
            const compilable::name_type &name,
            const boost::filesystem::path &executable,
            const std::vector<std::string> &flags_={});

        ProcessPointer create(
            const ProcessGroupPointer &process_group,
            const ProcessArguments &arguments) override;

    protected:
        virtual std::vector<std::string> arguments() const;
        virtual std::vector<std::string> flags() const;

    private:
        const boost::filesystem::path m_executable;
        const std::vector<std::string> m_flags;
    };
}}}
