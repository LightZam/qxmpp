/*
 *  Author:
 *      Zam Huang
 *  Brief:
 *      XEP-0055
 *      search from Jabber User Directory
 */

#ifndef QXMPPSEARCHMANAGER_H
#define QXMPPSEARCHMANAGER_H

#include "QXmppClientExtension.h"

class Item;

class QXMPP_EXPORT QXmppSearchIq : public QXmppIq {
public:

    class QXMPP_EXPORT Item {
    public:
        Item();

        QString jid() const;
        void setJid(const QString &jid);

        QString first() const;
        void setFirst(const QString &first);

        QString last() const;
        void setLast(const QString &last);

        QString nick() const;
        void setNick(const QString &nick);

        QString email() const;
        void setEmail(const QString &email);

        /// \cond
        void parse(const QDomElement &element);
        void toXml(QXmlStreamWriter *writer) const;
        /// \endcond

    private:
        QString m_jid;
        QString m_first;
        QString m_last;
        QString m_nick;
        QString m_email;
    };

    QXmppSearchIq();
    ~QXmppSearchIq();

    QXmppDataForm form() const;
    QXmppDataForm &form();
    void setForm(const QXmppDataForm &form);

    QString instructions() const;
    void setInstructions(const QString &instructions);

    QList<QXmppSearchIq::Item> items() const;
    QList<QXmppSearchIq::Item> &items();
    void setItems(const QList<QXmppSearchIq::Item> &value);

    QStringList fields() const;
    void setFields(const QStringList &fields);

    static bool isSearchIq(const QDomElement &element);
    void parseElementFromChild(const QDomElement &element);
    void toXmlElementFromChild(QXmlStreamWriter *writer) const;

private:
    QXmppDataForm m_form;
    QString m_instructions;
    QList<QXmppSearchIq::Item> m_items;
    QStringList m_fields;
};

class QXmppSearchManager : public QXmppClientExtension {
    Q_OBJECT
public:
    QXmppSearchManager();
    ~QXmppSearchManager();
    QString searchDomain();

    void searchUser(const QString &searchKey);
    void searchUser(QXmppSearchIq &iq);
    void searchFields(QXmppSearchIq &iq);

    /// \cond
    bool handleStanza(const QDomElement &stanza);
    /// \endcond

signals:
    void fieldsReceived(const QXmppSearchIq &iq);
    void itemsReceived(const QXmppSearchIq &iq);
};


#endif // QXMPPSEARCHMANAGER_H
