/*
    Copyright 2015 Jan Grulich <jgrulich@redhat.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sstpwidget.h"

#include "ui_sstpwidget.h"
#include "ui_sstpadvanced.h"

#include <QString>
#include <QDialog>
#include <QDialogButtonBox>

#include "nm-sstp-service.h"

class SstpSettingWidgetPrivate
{
public:
    Ui_SstpWidget ui;
    Ui_SstpAdvanced advUi;
    NetworkManager::VpnSetting::Ptr setting;
    QDialog *advancedDlg;
    QWidget *advancedWid;
};

SstpSettingWidget::SstpSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : SettingWidget(setting, parent)
    , d_ptr(new SstpSettingWidgetPrivate)
{
    Q_D(SstpSettingWidget);
    d->ui.setupUi(this);

    d->setting = setting;

    connect(d->ui.btn_advancedOption, &QPushButton::clicked, this, &SstpSettingWidget::doAdvancedDialog);
    connect(d->ui.cmb_passwordOption, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SstpSettingWidget::passwordTypeChanged);
    connect(d->ui.chk_showPassword, &QCheckBox::toggled, this, &SstpSettingWidget::setShowPassword);

    d->advancedDlg = new QDialog(this);
    d->advancedDlg->setModal(true);
    d->advancedWid = new QWidget(this);
    d->advUi.setupUi(d->advancedWid);
    QVBoxLayout *layout = new QVBoxLayout(d->advancedDlg);
    layout->addWidget(d->advancedWid);
    d->advancedDlg->setLayout(layout);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, d->advancedDlg);
    connect(buttons, &QDialogButtonBox::accepted, d->advancedDlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, d->advancedDlg, &QDialog::reject);
    connect(d->advUi.chk_showPassword, &QCheckBox::toggled, this, &SstpSettingWidget::setShowProxyPassword);

    layout->addWidget(buttons);

    connect(d->ui.le_gateway, &QLineEdit::textChanged, this, &SstpSettingWidget::slotWidgetChanged);
    connect(d->ui.le_username, &QLineEdit::textChanged, this, &SstpSettingWidget::slotWidgetChanged);
    connect(d->ui.le_password, &QLineEdit::textChanged, this, &SstpSettingWidget::slotWidgetChanged);

    KAcceleratorManager::manage(this);

    if (d->setting) {
        loadConfig(d->setting);
    }
}

SstpSettingWidget::~SstpSettingWidget()
{
    delete d_ptr;
}

void SstpSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_D(SstpSettingWidget);
    Q_UNUSED(setting)

    const QString yesString = QLatin1String("yes");
    const NMStringMap dataMap = d->setting->data();
    const NMStringMap secrets = d->setting->secrets();

    // General
    const QString gateway = dataMap[QLatin1String(NM_SSTP_KEY_GATEWAY)];
    if (!gateway.isEmpty()) {
        d->ui.le_gateway->setText(gateway);
    }

    // Optional setting
    const QString username = dataMap[QLatin1String(NM_SSTP_KEY_USER)];
    if (!username.isEmpty()) {
        d->ui.le_username->setText(username);
    }

    // Authentication
    const NetworkManager::Setting::SecretFlags type = (NetworkManager::Setting::SecretFlags)dataMap[NM_SSTP_KEY_PASSWORD_FLAGS].toInt();
    if (type & NetworkManager::Setting::AgentOwned || type == NetworkManager::Setting::None) {
        d->ui.le_password->setText(secrets[QLatin1String(NM_SSTP_KEY_PASSWORD)]);
    }
    fillOnePasswordCombo(d->ui.cmb_passwordOption, type);

    const QString ntDomain = dataMap[QLatin1String(NM_SSTP_KEY_DOMAIN)];
    if (!ntDomain.isEmpty()) {
        d->ui.le_ntDomain->setText(ntDomain);
    }

    const QString caCert = dataMap[QLatin1String(NM_SSTP_KEY_CA_CERT)];
    if (!caCert.isEmpty()) {
        d->ui.kurl_caCert->setUrl(QUrl::fromLocalFile(caCert));
    }

    const bool ignoreCertWarnings = (dataMap[QLatin1String(NM_SSTP_KEY_IGN_CERT_WARN)] == yesString);
    d->ui.chk_ignoreCertWarnings->setChecked(ignoreCertWarnings);

    // Advanced options - Point-to-Point
    bool refuse_pap = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_PAP)] == yesString);
    bool refuse_chap = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_CHAP)] == yesString);
    bool refuse_mschap = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAP)] == yesString);
    bool refuse_mschapv2 = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAPV2)] == yesString);
    bool refuse_eap = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_EAP)] == yesString);

    QListWidgetItem *item = 0;
    item = d->advUi.listWidget->item(0); // PAP
    item->setCheckState(refuse_pap ? Qt::Unchecked : Qt::Checked);
    item = d->advUi.listWidget->item(1); // CHAP
    item->setCheckState(refuse_chap ? Qt::Unchecked : Qt::Checked);
    item = d->advUi.listWidget->item(2); // MSCHAP
    item->setCheckState(refuse_mschap ? Qt::Unchecked : Qt::Checked);
    item = d->advUi.listWidget->item(3); // MSCHAPv2
    item->setCheckState(refuse_mschapv2 ? Qt::Unchecked : Qt::Checked);
    item = d->advUi.listWidget->item(4); // EAP
    item->setCheckState(refuse_eap ? Qt::Unchecked : Qt::Checked);

    // Cryptography and compression
    const bool mppe = (dataMap[QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE)] == yesString);
    const bool mppe40 = (dataMap[QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_40)] == yesString);
    const bool mppe128 = (dataMap[QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_128)] == yesString);
    const bool mppe_stateful = (dataMap[QLatin1String(NM_SSTP_KEY_MPPE_STATEFUL)] == yesString);

    if (mppe || mppe40 || mppe128) { // If MPPE is use
        d->advUi.gb_MPPE->setChecked(mppe || mppe40 || mppe128);
        if (mppe128) {
            d->advUi.cb_MPPECrypto->setCurrentIndex(1); // 128 bit
        } else if (mppe40) {
            d->advUi.cb_MPPECrypto->setCurrentIndex(2); // 40 bit
        } else {
            d->advUi.cb_MPPECrypto->setCurrentIndex(0); // Any
        }
        d->advUi.cb_statefulEncryption->setChecked(mppe_stateful);
    }

    const bool nobsd = (dataMap[QLatin1String(NM_SSTP_KEY_NOBSDCOMP)] == yesString);
    d->advUi.cb_BSD->setChecked(!nobsd);

    const bool nodeflate = (dataMap[QLatin1String(NM_SSTP_KEY_NODEFLATE)] == yesString);
    d->advUi.cb_deflate->setChecked(!nodeflate);

    const bool novjcomp = (dataMap[QLatin1String(NM_SSTP_KEY_NO_VJ_COMP)] == yesString);
    d->advUi.cb_TCPheaders->setChecked(!novjcomp);

    // Echo
    const int lcp_echo_interval = QString(dataMap[QLatin1String(NM_SSTP_KEY_LCP_ECHO_INTERVAL)]).toInt();
    d->advUi.cb_sendEcho->setChecked(lcp_echo_interval > 0);

    if (dataMap.contains(QLatin1String(NM_SSTP_KEY_UNIT_NUM))) {
        d->advUi.chk_useCustomUnitNumber->setChecked(true);
        d->advUi.sb_customUnitNumber->setValue(dataMap[QLatin1String(NM_SSTP_KEY_UNIT_NUM)].toInt());
    }

    // Advanced options - Proxy
    const QString address = dataMap[QLatin1String(NM_SSTP_KEY_PROXY_SERVER)];
    if (!address.isEmpty()) {
        d->advUi.le_address->setText(address);
    }

    const int port = dataMap[QLatin1String(NM_SSTP_KEY_PROXY_PORT)].toInt();
    if (port >= 0) {
        d->advUi.sb_port->setValue(port);
    }

    const QString proxyUsername = dataMap[QLatin1String(NM_SSTP_KEY_PROXY_USER)];
    if (!proxyUsername.isEmpty()) {
        d->advUi.le_username->setText(proxyUsername);
    }

    const QString proxyPassword = dataMap[QLatin1String(NM_SSTP_KEY_PROXY_PASSWORD)];
    if (!proxyPassword.isEmpty()) {
        d->advUi.le_password->setText(proxyPassword);
    }
}

QVariantMap SstpSettingWidget::setting(bool agentOwned) const
{
    Q_D(const SstpSettingWidget);

    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_SSTP));

    const QString yesString = QLatin1String("yes");

    NMStringMap data;
    NMStringMap secretData;

    data.insert(QLatin1String(NM_SSTP_KEY_GATEWAY),  d->ui.le_gateway->text());

    if (!d->ui.le_username->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_USER), d->ui.le_username->text());
    }

    if (!d->ui.le_password->text().isEmpty()) {
        secretData.insert(QLatin1String(NM_SSTP_KEY_PASSWORD), d->ui.le_password->text());
    }
    handleOnePasswordType(d->ui.cmb_passwordOption, NM_SSTP_KEY_PASSWORD_FLAGS, data, agentOwned);

    if (!d->ui.le_ntDomain->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_DOMAIN), d->ui.le_ntDomain->text());
    }

    if (!d->ui.kurl_caCert->url().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_CA_CERT), d->ui.kurl_caCert->url().toLocalFile());
    }

    if (d->ui.chk_ignoreCertWarnings->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_IGN_CERT_WARN), yesString);
    }

    // Advanced configuration
    QListWidgetItem *item = 0;
    item = d->advUi.listWidget->item(0); // PAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_PAP), yesString);
    }
    item = d->advUi.listWidget->item(1); // CHAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_CHAP), yesString);
    }
    item = d->advUi.listWidget->item(2); // MSCHAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAP), yesString);
    }
    item = d->advUi.listWidget->item(3); // MSCHAPv2
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAPV2), yesString);
    }
    item = d->advUi.listWidget->item(4); // EAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_EAP), yesString);
    }

    // Cryptography and compression
    if (d->advUi.gb_MPPE->isChecked()) {
        int index = d->advUi.cb_MPPECrypto->currentIndex();

        switch (index) {
        case 0:
            data.insert(QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE), yesString);
            break;
        case 1:
            data.insert(QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_128), yesString);
            break;
        case 2:
            data.insert(QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_40), yesString);
            break;
        }

        if (d->advUi.cb_statefulEncryption->isChecked()) {
            data.insert(NM_SSTP_KEY_MPPE_STATEFUL, yesString);
        }
    }

    if (!d->advUi.cb_BSD->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_NOBSDCOMP), yesString);
    }

    if (!d->advUi.cb_deflate->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_NODEFLATE), yesString);
    }

    if (!d->advUi.cb_TCPheaders->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_NO_VJ_COMP), yesString);
    }

    // Echo
    if (d->advUi.cb_sendEcho->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_LCP_ECHO_FAILURE), "5");
        data.insert(QLatin1String(NM_SSTP_KEY_LCP_ECHO_INTERVAL), "30");
    }

    if (d->advUi.chk_useCustomUnitNumber->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_UNIT_NUM), QString::number(d->advUi.sb_customUnitNumber->value()));
    }

    if (!d->advUi.le_address->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_SERVER), d->advUi.le_address->text());
    }

    if (d->advUi.sb_port->value() >= 0) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_PORT), QString::number(d->advUi.sb_port->value()));
    }

    if (!d->advUi.le_username->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_USER), d->advUi.le_username->text());
    }

    if (!d->advUi.le_password->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_PASSWORD), d->advUi.le_password->text());
        if (agentOwned) {
            data.insert(QLatin1String(NM_SSTP_KEY_PROXY_PASSWORD_FLAGS), QString::number(NetworkManager::Setting::AgentOwned));
        } else {
            data.insert(QLatin1String(NM_SSTP_KEY_PROXY_PASSWORD_FLAGS), QString::number(NetworkManager::Setting::None));
        }
    }

    // save it all
    setting.setData(data);
    setting.setSecrets(secretData);

    return setting.toMap();
}

void SstpSettingWidget::doAdvancedDialog()
{
    Q_D(SstpSettingWidget);
    d->advancedDlg->show();
}

void SstpSettingWidget::passwordTypeChanged(int index)
{
    Q_D(SstpSettingWidget);
    d->ui.le_password->setEnabled(index == SettingWidget::EnumPasswordStorageType::Store);
}

void SstpSettingWidget::setShowPassword(bool show)
{
    Q_D(SstpSettingWidget);
    if (show) {
        d->ui.le_password->setEchoMode(QLineEdit::Normal);
    } else {
        d->ui.le_password->setEchoMode(QLineEdit::Password);
    }
}

void SstpSettingWidget::setShowProxyPassword(bool show)
{
    Q_D(SstpSettingWidget);
    if (show) {
        d->advUi.le_password->setEchoMode(QLineEdit::Normal);
    } else {
        d->advUi.le_password->setEchoMode(QLineEdit::Password);
    }
}

void SstpSettingWidget::fillOnePasswordCombo(QComboBox *combo, NetworkManager::Setting::SecretFlags type)
{
    if (type.testFlag(NetworkManager::Setting::AgentOwned) || type == NetworkManager::Setting::None) { // store
        combo->setCurrentIndex(SettingWidget::EnumPasswordStorageType::Store);
    } else if (type.testFlag(NetworkManager::Setting::NotSaved)) {
        combo->setCurrentIndex(SettingWidget::EnumPasswordStorageType::AlwaysAsk);
    } else if (type.testFlag(NetworkManager::Setting::NotRequired)) {
        combo->setCurrentIndex(SettingWidget::EnumPasswordStorageType::NotRequired);
    }
}

uint SstpSettingWidget::handleOnePasswordType(const QComboBox *combo, const QString &key, NMStringMap &data, bool agentOwned) const
{
    const uint type = combo->currentIndex();
    switch (type) {
    case SettingWidget::EnumPasswordStorageType::Store:
        if (agentOwned) {
            data.insert(key, QString::number(NetworkManager::Setting::AgentOwned)); // store
        } else {
            data.insert(key, QString::number(NetworkManager::Setting::None));
        }
        break;
    case SettingWidget::EnumPasswordStorageType::AlwaysAsk:
        data.insert(key, QString::number(NetworkManager::Setting::NotSaved)); // always ask
        break;
    case SettingWidget::EnumPasswordStorageType::NotRequired:
        data.insert(key, QString::number(NetworkManager::Setting::NotRequired)); // not required
        break;
    }
    return type;
}

bool SstpSettingWidget::isValid() const
{
    Q_D(const SstpSettingWidget);

    return !d->ui.le_gateway->text().isEmpty();
}
