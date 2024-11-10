#pragma once
#include <vector>
#include <gtkmm.h>
#include "searchprovider_stub.h"

class SearchProvider : public org::gnome::Shell::SearchProvider2Stub
{
    virtual void
    GetInitialResultSet(const std::vector<Glib::ustring> & terms,
                        MethodInvocation &invocation) override final;


    virtual void
    GetSubsearchResultSet(const std::vector<Glib::ustring> & previous_results,
                          const std::vector<Glib::ustring> & terms,
                          MethodInvocation &invocation) override final;

    virtual void
    GetResultMetas(const std::vector<Glib::ustring> & identifiers,
                   MethodInvocation &invocation) override final;

    virtual void
    ActivateResult(const Glib::ustring & identifier,
                   const std::vector<Glib::ustring> & terms,
                   guint32 timestamp,
                   MethodInvocation &invocation) override final;

    virtual void
    LaunchSearch(const std::vector<Glib::ustring> & terms,
                 guint32 timestamp,
                 MethodInvocation &invocation) override final;

public:
    sigc::signal<void(Glib::ustring)> signal_activate;
};
