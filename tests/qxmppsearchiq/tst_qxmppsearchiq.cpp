/*
 *  Author:
 *      Zam Huang
 *  Brief:
 *      XEP-0055
 *      search from Jabber User Directory
 */

#include <QObject>
#include <QtTest>
#include "util.h"
#include "QXmppSearchManager.h"

class tst_Qxmppsearchiq : public QObject
{
    Q_OBJECT

private slots:
    void testSimpleSearchFileds();
    void testSimpleSearchUser();
    void testSearchFields();
    void testSearchUser();
};

void tst_Qxmppsearchiq::testSimpleSearchFileds()
{
    const QByteArray xml(
        "<iq id=\"search1\" to=\"romeo@montague.net/home\" from=\"characters.shakespeare.lit\" type=\"result\">"
          "<query xmlns=\"jabber:iq:search\">"
                "<instructions>"
                  "Fill in one or more fields to search for any matching Jabber users."
                "</instructions>"
                "<first/>"
                "<last/>"
                "<nick/>"
                "<email/>"
          "</query>"
        "</iq>");

    QXmppSearchIq iq;
    parsePacket(iq, xml);
    QCOMPARE(iq.instructions(), QString("Fill in one or more fields to search for any matching Jabber users."));
    QCOMPARE(iq.fields().isEmpty(), false);
    QCOMPARE(iq.fields().size(), 4);
    QCOMPARE(iq.fields().at(0), QString("first"));
    QCOMPARE(iq.fields().at(1), QString("last"));
    QCOMPARE(iq.fields().at(2), QString("nick"));
    QCOMPARE(iq.fields().at(3), QString("email"));

    serializePacket(iq, xml);
}

void tst_Qxmppsearchiq::testSimpleSearchUser()
{
    const QByteArray xml(
        "<iq id=\"search2\" to=\"romeo@montague.net/home\" from=\"characters.shakespeare.lit\" type=\"result\">"
            "<query xmlns=\"jabber:iq:search\">"
                "<item jid=\"juliet@capulet.com\">"
                    "<first>Juliet</first>"
                    "<last>Capulet</last>"
                    "<nick>JuliC</nick>"
                    "<email>juliet@shakespeare.lit</email>"
                "</item>"
                "<item jid=\"tybalt@capulet.com\">"
                    "<first>Tybalt</first>"
                    "<last>Capulet</last>"
                    "<nick>ty</nick>"
                    "<email>tybalt@shakespeare.lit</email>"
                "</item>"
            "</query>"
        "</iq>");

    QXmppSearchIq iq;
    parsePacket(iq, xml);
    QCOMPARE(iq.items().size(), 2);

    QXmppSearchIq::Item item1 = iq.items().at(0);
    QCOMPARE(item1.jid(), QString("juliet@capulet.com"));
    QCOMPARE(item1.first(), QString("Juliet"));
    QCOMPARE(item1.last(), QString("Capulet"));
    QCOMPARE(item1.nick(), QString("JuliC"));
    QCOMPARE(item1.email(), QString("juliet@shakespeare.lit"));

    QXmppSearchIq::Item item2 = iq.items().at(1);
    QCOMPARE(item2.jid(), QString("tybalt@capulet.com"));
    QCOMPARE(item2.first(), QString("Tybalt"));
    QCOMPARE(item2.last(), QString("Capulet"));
    QCOMPARE(item2.nick(), QString("ty"));
    QCOMPARE(item2.email(), QString("tybalt@shakespeare.lit"));

    serializePacket(iq, xml);
}

void tst_Qxmppsearchiq::testSearchFields()
{
    QByteArray xml(
        "<iq id=\"search3\" to=\"juliet@capulet.com/balcony\" from=\"characters.shakespeare.lit\" type=\"result\">"
          "<query xmlns=\"jabber:iq:search\">"
            "<instructions>"
              "Use the enclosed form to search. If your Jabber client does not support Data Forms, visit http://shakespeare.lit/"
            "</instructions>"
            "<x xmlns=\"jabber:x:data\" type=\"form\">"
              "<title>User Directory Search</title>"
              "<instructions>"
                "Please provide the following information to search for Shakespearean characters."
              "</instructions>"
              "<field type=\"hidden\" var=\"FORM_TYPE\">"
                "<value>jabber:iq:search</value>"
              "</field>"
              "<field type=\"text-single\" label=\"Given Name\" var=\"first\"/>"
              "<field type=\"text-single\" label=\"Family Name\" var=\"last\"/>"
              "<field type=\"list-single\" label=\"Gender\" var=\"x-gender\">"
                "<option label=\"Male\"><value>male</value></option>"
                "<option label=\"Female\"><value>female</value></option>"
              "</field>"
            "</x>"
          "</query>"
        "</iq>");

    QXmppSearchIq iq;
    parsePacket(iq, xml);
    QCOMPARE(iq.instructions(), QString("Use the enclosed form to search. If your Jabber client does not support Data Forms, visit http://shakespeare.lit/"));

    QXmppDataForm form = iq.form();
    QCOMPARE(form.isNull(), false);
    QCOMPARE(form.fields().size(), 4);
    QCOMPARE(form.title(), QString("User Directory Search"));
    QCOMPARE(form.instructions(), QString("Please provide the following information to search for Shakespearean characters."));

    QXmppDataForm::Field hiddenField = form.fields().at(0);
    QCOMPARE(hiddenField.key(), QString("FORM_TYPE"));
    QCOMPARE(hiddenField.type(), QXmppDataForm::Field::HiddenField);
    QCOMPARE(hiddenField.value().toString(), QString("jabber:iq:search"));

    QXmppDataForm::Field firstField = form.fields().at(1);
    QCOMPARE(firstField.key(), QString("first"));
    QCOMPARE(firstField.type(), QXmppDataForm::Field::TextSingleField);
    QCOMPARE(firstField.label(), QString("Given Name"));

    QXmppDataForm::Field lastNameField = form.fields().at(2);
    QCOMPARE(lastNameField.key(), QString("last"));
    QCOMPARE(lastNameField.type(), QXmppDataForm::Field::TextSingleField);
    QCOMPARE(lastNameField.label(), QString("Family Name"));

    QXmppDataForm::Field genderField = form.fields().at(3);
    QCOMPARE(genderField.key(), QString("x-gender"));
    QCOMPARE(genderField.type(), QXmppDataForm::Field::ListSingleField);
    QCOMPARE(genderField.label(), QString("Gender"));

    serializePacket(iq, xml);
}

void tst_Qxmppsearchiq::testSearchUser()
{
    const QByteArray xml(
        "<iq id=\"search4\" to=\"juliet@capulet.com/balcony\" from=\"characters.shakespeare.lit\" type=\"result\">"
          "<query xmlns=\"jabber:iq:search\">"
            "<x xmlns=\"jabber:x:data\" type=\"result\">"
                "<field type=\"hidden\" var=\"FORM_TYPE\">"
                    "<value>jabber:iq:search</value>"
                "</field>"
                "<reported>"
                    "<field type=\"jid-single\" label=\"JID\" var=\"jid\"/>"
                    "<field type=\"text-single\" label=\"Username\" var=\"Username\"/>"
                    "<field type=\"text-single\" label=\"Name\" var=\"Name\"/>"
                    "<field type=\"text-single\" label=\"Email\" var=\"Email\"/>"
                "</reported>"
                "<item>"
                    "<field type=\"text-single\" var=\"jid\">"
                        "<value>Zam@im-BestTaiwan.com.tw</value>"
                    "</field>"
                    "<field type=\"text-single\" var=\"Username\">"
                        "<value>Zam</value>"
                    "</field>"
                    "<field type=\"text-single\" var=\"Name\">"
                        "<value>Zam</value>"
                    "</field>"
                    "<field type=\"text-single\" var=\"Email\">"
                        "<value>Zam@BestTaiwan.com.tw</value>"
                    "</field>"
                "</item>"
            "</x>"
          "</query>"
        "</iq>");
    QXmppSearchIq iq;
    parsePacket(iq, xml);
    QXmppDataForm form = iq.form();
    QCOMPARE(form.isNull(), false);
    QCOMPARE(form.fields().size(), 1);

    const QString KEY_JID = "jid";
    const QString KEY_USERNAME = "Username";
    const QString KEY_NAME = "Name";
    const QString KEY_EMAIL = "Email";
    /* reported tag test */
    QCOMPARE(form.reported().fields().size(), 4);
    QXmppDataForm::Field jidField = form.reported().fields().at(0);
    QCOMPARE(jidField.key(), KEY_JID);
    QCOMPARE(jidField.type(), QXmppDataForm::Field::JidSingleField);
    QCOMPARE(jidField.label(), QString("JID"));

    QXmppDataForm::Field usernameField = form.reported().fields().at(1);
    QCOMPARE(usernameField.key(), KEY_USERNAME);
    QCOMPARE(usernameField.type(), QXmppDataForm::Field::TextSingleField);
    QCOMPARE(usernameField.label(), QString("Username"));

    QXmppDataForm::Field nameField = form.reported().fields().at(2);
    QCOMPARE(nameField.key(), KEY_NAME);
    QCOMPARE(nameField.type(), QXmppDataForm::Field::TextSingleField);
    QCOMPARE(nameField.label(), QString("Name"));

    QXmppDataForm::Field emailField = form.reported().fields().at(3);
    QCOMPARE(emailField.key(), KEY_EMAIL);
    QCOMPARE(emailField.type(), QXmppDataForm::Field::TextSingleField);
    QCOMPARE(emailField.label(), QString("Email"));

    /* item tag test */
    QCOMPARE(form.items().size(), 1);
    QXmppDataForm::Item item = form.items().at(0);
    QCOMPARE(item.fields().size(), 4);

    QXmppDataForm::Field jidItemField = item.fields().at(0);
    QCOMPARE(jidItemField.key(), KEY_JID);
    QCOMPARE(jidItemField.value().toString(), QString("Zam@im-BestTaiwan.com.tw"));

    QXmppDataForm::Field usernameItemField = item.fields().at(1);
    QCOMPARE(usernameItemField.key(), KEY_USERNAME);
    QCOMPARE(usernameItemField.value().toString(), QString("Zam"));

    QXmppDataForm::Field nameItemField = item.fields().at(2);
    QCOMPARE(nameItemField.key(), KEY_NAME);
    QCOMPARE(nameItemField.value().toString(), QString("Zam"));

    QXmppDataForm::Field emailItemField = item.fields().at(3);
    QCOMPARE(emailItemField.key(), KEY_EMAIL);
    QCOMPARE(emailItemField.value().toString(), QString("Zam@BestTaiwan.com.tw"));

    serializePacket(iq, xml);
}

QTEST_MAIN(tst_Qxmppsearchiq)

#include "tst_qxmppsearchiq.moc"
