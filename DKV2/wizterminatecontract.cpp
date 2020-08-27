
#include <QLabel>
#include <QDateEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QMessageBox>

#include "wizterminatecontract.h"

wizTerminateContract_DatePage::wizTerminateContract_DatePage(QWidget* p) : QWizardPage(p)
{
    setTitle(qsl("Vertrag beenden"));
    setSubTitle(qsl("Mit dieser Dialogfolge kannst Du einen Vertrag beenden.<p>"
                "Gib das Datum an, zu dem der Vertrag ausgezahlt wird. "
                "Bis zu diesem Datum werden die Zinsen berechnet. "));
    QDateEdit* de = new QDateEdit;
    de->setDisplayFormat(qsl("dd.MM.yyyy"));
    registerField(qsl("date"), de);
    de->setToolTip(qsl("Rückerstattungsdatum"));
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(de);
    setLayout(layout);
}

void wizTerminateContract_DatePage::initializePage()
{
    wizTerminateContract* wiz = qobject_cast<wizTerminateContract*>(wizard());
    setField(qsl("date"), wiz->c.plannedEndDate());
}

bool wizTerminateContract_DatePage::validatePage()
{
    wizTerminateContract* wiz = qobject_cast<wizTerminateContract*>(wizard());
    if( field(qsl("date")).toDate() >= wiz->c.latestBooking().date)
        return true;
    QString msg (qsl("Das Vertragsende muss nach der letzten Buchung des Vertrags am %1 sein"));
    msg = msg.arg(wiz->c.latestBooking().date.toString(qsl("dd.MM.yyyy")));
    QMessageBox::information(this, qsl("Ungültiges Datum"), msg);
    return false;
}

wizTerminateContract_ConfirmationPage::wizTerminateContract_ConfirmationPage(QWidget* p) : QWizardPage(p)
{
    setTitle(qsl("Vertrag beenden"));
    QCheckBox* cbPrint = new QCheckBox(qsl("Beleg als CSV Datei speichern"));
    registerField(qsl("print"), cbPrint);
    cbPrint->setToolTip(qsl("Die Datei wird in dem konfigurierten Ausgabeordner gespeichert"));
    QCheckBox* cbConfirm = new QCheckBox(qsl("Die Engaben sind korrekt"));
    registerField(qsl("confirm"), cbConfirm);
    cbConfirm->setToolTip(qsl("Bestätige, dass die Eingaben richtig sind!"));
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(cbPrint);
    layout->addWidget(cbConfirm);
    setLayout(layout);
    connect(cbConfirm, SIGNAL(stateChanged(int)), this, SLOT(onConfirmData_toggled(int)));
}

void wizTerminateContract_ConfirmationPage::initializePage()
{
    wizTerminateContract* wiz = qobject_cast<wizTerminateContract*>(wizard());
    double interest =0., finalValue =0.;
    wiz->c.finalize(true, field(qsl("date")).toDate(), interest, finalValue);

    QString subtitle = qsl("<table width=100%>"
                       "<tr><td>Bewertung des Vertrags zum Laufzeitende</td><td align=right><b>%1</b> </td></tr>"
                       "<tr><td>Zinsen der letzten Zinsphase</td>            <td align=right><b>%2</b> </td></tr>"
                       "<tr><td>Auszahlungsbetrag </td>                      <td align=right><b>%3</b> </td></tr>"
                       "</table>");
    QLocale locale;
    subtitle = subtitle.arg(locale.toCurrencyString(wiz->c.value()), locale.toCurrencyString(interest), locale.toCurrencyString(finalValue));
    setSubTitle(subtitle);
}
void wizTerminateContract_ConfirmationPage::onConfirmData_toggled(int)
{
    completeChanged();
}
bool wizTerminateContract_ConfirmationPage::isComplete() const
{
    return field("confirm").toBool();
}

wizTerminateContract::wizTerminateContract(QWidget* p, contract c) : QWizard(p), c(c)
{
    addPage(new wizTerminateContract_DatePage);
    addPage(new wizTerminateContract_ConfirmationPage);
}
