#ifndef SwissalpS_QtSssSapp_ZEROCONFHELPERS_H
#define SwissalpS_QtSssSapp_ZEROCONFHELPERS_H

#include <QAbstractSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

#include "qzeroconf.h"



namespace SwissalpS { namespace QtSssSapp {



class ZeroConfServiceDescriptor : public QObject {

	Q_OBJECT

private:
	bool bIsBrowserInfo;
	bool bOK;
	QJsonObject ojoService;
	QZeroConf *pService;
	QString sModuleUID;

public:
	explicit ZeroConfServiceDescriptor(const QZeroConfService &oService);
	explicit ZeroConfServiceDescriptor(const QString &sModuleUID,
									   QJsonObject &ojoServiceDescriptor,
									   QObject *pParent = nullptr);
	virtual ~ZeroConfServiceDescriptor() {}

	QString domain() const;
	QString host() const;
	quint32 interfaceIndex() const;
	QString ip() const;
	QString ipv6() const;
	bool isActive() const;
	inline bool isNull() const { return !this->bOK; }
	inline QString moduleUID() const { return this->sModuleUID; }
	quint16 port() const;
	QString serviceName() const;
	inline QZeroConf *servicePointer() const { return this->pService; }
	inline void setServicePointer(QZeroConf *pPointer = nullptr) { this->pService = pPointer; }
	QJsonArray textRecords() const;
	inline QJsonObject toJSON() const { return this->bOK ? this->ojoService
														 : QJsonObject(); }
	QString type() const;

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("ZeroConfServiceDescriptor:" + sMessage); }

}; // ZeroConfServiceDescriptor



class ZeroConfBrowser : public QObject {

	Q_OBJECT
	Q_DISABLE_COPY(ZeroConfBrowser)

private:
	QAbstractSocket::NetworkLayerProtocol eProtocol;
	QZeroConf *pBrowser;
	QString sType;

protected:
	void init();

protected slots:
	void onError(QZeroConf::error_t eError);
	void onServiceAdded(QZeroConfService oService);
	void onServiceRemoved(QZeroConfService oService);
	void onServiceUpdated(QZeroConfService oService);

public:
	explicit ZeroConfBrowser(const QString sType, QObject *pParent = nullptr);
	explicit ZeroConfBrowser(const QString sType,
							 QAbstractSocket::NetworkLayerProtocol eProtocol,
							 QObject *pParent = nullptr);
	virtual ~ZeroConfBrowser() { this->stop(); }

	void start();
	void stop();

signals:
	void error() const;
	void serviceAdded(ZeroConfServiceDescriptor *pZCSD) const;
	void serviceRemoved(ZeroConfServiceDescriptor *pZCSD) const;
	void serviceUpdated(ZeroConfServiceDescriptor *pZCSD) const;
	void debugMessage(const QString &sMessage) const;

public slots:
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("ZeroConfBrowser:" + sMessage); }

}; // ZeroConfBrowser



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_ZEROCONFHELPERS_H
