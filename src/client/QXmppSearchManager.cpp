/*
 *  Author:
 *      Zam Huang
 *  Brief:
 *      XEP-0055
 *      search from Jabber User Directory
 */

#include <QDomElement>
#include <QUuid>

#include "QXmppSearchManager.h"
#include "QXmppClient.h"
#include "QXmppIq.h"
#include "QXmppUtils.h"
#include "QXmppConstants.h"

static const char *search_user_field = "user:search:field";

QXmppSearchIq::QXmppSearchIq() {
}

QXmppSearchIq::~QXmppSearchIq() {
}

QString QXmppSearchIq::instructions() const {
    return m_instructions;
}
void QXmppSearchIq::setInstructions(const QString &instructions) {
    m_instructions = instructions;
}

QXmppDataForm &QXmppSearchIq::form() {
    return m_form;
}

QXmppDataForm QXmppSearchIq::form() const {
    return m_form;
}

void QXmppSearchIq::setForm(const QXmppDataForm &form) {
    m_form = form;
}

QList<QXmppSearchIq::Item> QXmppSearchIq::items() const
{
    return m_items;
}

QList<QXmppSearchIq::Item> &QXmppSearchIq::items()
{
    return m_items;
}

QStringList QXmppSearchIq::fields() const
{
    return m_fields;
}

void QXmppSearchIq::setFields(const QStringList &fields)
{
    m_fields = fields;
}

bool QXmppSearchIq::isSearchIq(const QDomElement &element) {
    QDomElement queryElement = element.firstChildElement("query");
    return queryElement.namespaceURI() == ns_search;
}

void QXmppSearchIq::parseElementFromChild(const QDomElement &element) {
    QDomElement queryElement = element.firstChildElement("query");
    m_form.parse(queryElement.firstChildElement("x"));
    if (!queryElement.firstChildElement("instructions").isNull())
        m_instructions = queryElement.firstChildElement("instructions").text();

    QDomElement itemElement = queryElement.firstChildElement("item");
    while (!itemElement.isNull()) {
        QXmppSearchIq::Item item;
        item.parse(itemElement);
        m_items.append(item);
        itemElement = itemElement.nextSiblingElement("item");
    }
    if (m_form.fields().isEmpty() && m_items.isEmpty()) {
        QDomElement fieldElement = queryElement.firstChildElement();
        while (!fieldElement.isNull()) {
            if (fieldElement.text().isEmpty()) {
                m_fields.append(fieldElement.tagName());
            }
            fieldElement = fieldElement.nextSiblingElement();
        }
    }
}

void QXmppSearchIq::toXmlElementFromChild(QXmlStreamWriter *writer) const {
    writer->writeStartElement("query");
    helperToXmlAddAttribute(writer, "xmlns", ns_search);
    if (!m_instructions.isEmpty())
        helperToXmlAddTextElement(writer, "instructions", m_instructions);
    foreach (QString field, m_fields) {
        helperToXmlAddTextElement(writer, field, "");
    }
    foreach (QXmppSearchIq::Item item, m_items) {
        item.toXml(writer);
    }
    m_form.toXml(writer);
    writer->writeEndElement();
}

void QXmppSearchIq::setItems(const QList<QXmppSearchIq::Item> &value)
{
    m_items = value;
}

QXmppSearchIq::Item::Item() {
}

QString QXmppSearchIq::Item::jid() const {
    return m_jid;
}

void QXmppSearchIq::Item::setJid(const QString &jid) {
    m_jid = jid;
}

QString QXmppSearchIq::Item::first() const {
    return m_first;
}

void QXmppSearchIq::Item::setFirst(const QString &first) {
    m_first = first;
}

QString QXmppSearchIq::Item::last() const {
    return m_last;
}

void QXmppSearchIq::Item::setLast(const QString &last) {
    m_last = last;
}

QString QXmppSearchIq::Item::nick() const {
    return m_nick;
}

void QXmppSearchIq::Item::setNick(const QString &nick) {
    m_nick = nick;
}

QString QXmppSearchIq::Item::email() const {
    return m_email;
}

void QXmppSearchIq::Item::setEmail(const QString &email) {
    m_email = email;
}

void QXmppSearchIq::Item::parse(const QDomElement &element) {
    if (!element.attribute("jid").isNull())
        m_jid = element.attribute("jid");
    if (!element.firstChildElement("first").isNull())
        m_first = element.firstChildElement("first").text();
    if (!element.firstChildElement("last").isNull())
        m_last = element.firstChildElement("last").text();
    if (!element.firstChildElement("nick").isNull())
        m_nick = element.firstChildElement("nick").text();
    if (!element.firstChildElement("email").isNull())
        m_email = element.firstChildElement("email").text();
}

void QXmppSearchIq::Item::toXml(QXmlStreamWriter *writer) const {
    writer->writeStartElement("item");
    helperToXmlAddAttribute(writer, "jid", m_jid);
    if (!m_first.isEmpty())
        helperToXmlAddTextElement(writer, "first", m_first);
    if (!m_last.isEmpty())
        helperToXmlAddTextElement(writer, "last", m_last);
    if (!m_nick.isEmpty())
        helperToXmlAddTextElement(writer, "nick", m_nick);
    if (!m_email.isEmpty())
        helperToXmlAddTextElement(writer, "email", m_email);
    writer->writeEndElement();
}

QXmppSearchManager::QXmppSearchManager() {
}

QXmppSearchManager::~QXmppSearchManager() {
}

QString QXmppSearchManager::searchDomain() {
    return "search." + client()->configuration().domain();
}

// search user for openfire server
void QXmppSearchManager::searchUser(const QString &searchKey) {
    QXmppSearchIq iq;
    QXmppDataForm form;
    QList<QXmppDataForm::Field> fields;
    QXmppDataForm::Field search, username, name, email;

    search.setKey("search");
    search.setValue(searchKey);
    username.setKey("Username");
    username.setValue(1);
    name.setKey("Name");
    name.setValue(1);
    email.setKey("Email");
    email.setValue(1);

    fields.append(search);
    fields.append(username);
    fields.append(name);
    fields.append(email);
    form.setType(QXmppDataForm::Submit);
    form.setFields(fields);
    iq.setForm(form);
    searchUser(iq);
}

void QXmppSearchManager::searchUser(QXmppSearchIq &iq) {
    iq.setType(QXmppIq::Set);
    iq.setTo(searchDomain());
    client()->sendPacket(iq);
}

void QXmppSearchManager::searchFields(QXmppSearchIq &iq) {
    QString id = search_user_field + QUuid::createUuid().toString();
    iq.setId(id);
    iq.setType(QXmppIq::Get);
    iq.setTo(searchDomain());
    client()->sendPacket(iq);
}

bool QXmppSearchManager::handleStanza(const QDomElement &stanza) {
    if (stanza.tagName() == "iq") {
        if (QXmppSearchIq::isSearchIq(stanza)) {
            QXmppSearchIq iq;
            iq.parse(stanza);
            if (iq.type() == QXmppIq::Result) {
                if (iq.id().contains(search_user_field)) {
                    emit fieldsReceived(iq);
                } else {
                    emit itemsReceived(iq);
                }
                return true;
            }
        }
    }
    return false;
}
