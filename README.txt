# CSafeObject

Simple C++14 thread object, queue and stack key

This project in order to achieve a simple reusable thread container.
OK, I admit that I do not understand what the ShenMeGui smart pointer, so try to implement a thread container, of course, performance may not be better than the smart pointer, purely entertainment

Example：
...
#include "CodePlus/CSafeMap.hpp"
...

CSafeObject<CSqlite*>	m_scSqlite;

int main()
{
  m_scSqlite.CreateObject([&]() ->CSqlite* {
		CSqlite*sql = new CSqlite;
		sql->InitDB(wstring(GetMainPathW(_T("usr/junfeng.db"))),
		{
		_T("create table Info ( id,wmsCode,tid,orderCode,status,wmsStatus,isQuickCodOrder,sellerNick,buyerNick,deliveryStart,deliveryDate,outSid,itemTitle,receiverName,receiverPhone,receiverMobile,zip,address,state,city,district,type,freightPayer,sellerConfirm,companyName,startingSiteCode,isSuccess,created,modified,isSpilt,subTids,quantity,sellerId,sellerName,salesmanId,salesmanName,printInvoiceDate,toDate,printExpressNoDate,dataSources,importId,waybillType );"),
		_T("create table Express (id,name,code,taobaoId,regMailNo,status,dataType,printStatus,sort) ")
		});
		return sql;
	});

  m_scSqlite.CtrlItem([&](CSqlite*tmp) 
  {
		wstring wscSql = _T("select * from Info ");
		vector<map<CString, CString>> data;
		tmp->_sqlite3_prepare(wscSql, data);
		if (data.size() == 0) return;
		m_mapCodeInfo = *data.begin();
		HandleModuleSettings();
		bret = true;
		NowLogStringEx(_T("end 1"));
}
	);
	
	m_scSqlite.Delete();
	
// Here we expression by C++14 lambal to control object,
// good in it, I will tell you is actually in front of the call lambal expressions use a mutex and a counter, 
// and then after the implementation will release mu'te'x and restore counter, ultra low energy approach。

//Note that when you want to use multiple CSafeXX, do not use nested, or a certain chance will appear, 
//You'd better use the copy object to avoid nested use.

}

