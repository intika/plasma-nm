add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_sshui\")

set(ssh_SRCS
   ssh.cpp
   sshwidget.cpp
   sshauth.cpp
   )

ki18n_wrap_ui(ssh_SRCS sshwidget.ui sshadvanced.ui sshauth.ui)

add_library(plasmanetworkmanagement_sshui ${ssh_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_sshui plasmanetworkmanagement_sshui.desktop)

target_link_libraries(plasmanetworkmanagement_sshui
    plasmanm_internal
    plasmanm_editor
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::NetworkManagerQt
    KF5::Service
    KF5::Completion
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOWidgets
    KF5::CoreAddons
)

install(TARGETS plasmanetworkmanagement_sshui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_sshui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
