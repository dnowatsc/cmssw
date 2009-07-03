#include "DataFormats/Provenance/interface/LuminosityBlockID.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CondCore/DBCommon/interface/Time.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/SessionConfiguration.h"
#include "CondCore/DBCommon/interface/ConnectionConfiguration.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondCore/DBCommon/interface/Connection.h"
#include "CondCore/DBCommon/interface/CoralTransaction.h"
#include "CondCore/DBCommon/src/CoralConnectionProxy.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/Exception.h"
#include "CoralBase/TimeStamp.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITypeConverter.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "HLTScalerDBReader.h"
#include "CondTools/RunInfo/interface/HLTScalerReaderFactory.h"
//#include <iostream>
lumi::HLTScalerDBReader::HLTScalerDBReader(const edm::ParameterSet&pset):lumi::HLTScalerReaderBase(pset),m_session(new cond::DBSession ){
  m_constr=pset.getParameter<std::string>("connect");
  std::string authPath=pset.getParameter<std::string>("authenticationPath");
  int messageLevel=pset.getUntrackedParameter<int>("messageLevel",0);
  switch (messageLevel) {
  case 0 :
    m_session->configuration().setMessageLevel( cond::Error );
    break;    
  case 1:
    m_session->configuration().setMessageLevel( cond::Warning );
    break;
  case 2:
    m_session->configuration().setMessageLevel( cond::Info );
    break;
  case 3:
    m_session->configuration().setMessageLevel( cond::Debug );
    break;  
  default:
    m_session->configuration().setMessageLevel( cond::Error );
  }
  m_session->configuration().setAuthenticationMethod(cond::XML);
  m_session->configuration().setAuthenticationPath(authPath);
}
lumi::HLTScalerDBReader::~HLTScalerDBReader(){
  delete m_session;
}

void 
lumi::HLTScalerDBReader::fill(int startRun,
			   int numberOfRuns,
			   std::vector< std::pair<lumi::HLTScaler*,cond::Time_t> >& result){
  //fill hlt scaler info with 2 queries
  //select count(distinct PATHNAME ) as npath from HLT_SUPERVISOR_LUMISECTIONS_V2;
  //select l.PATHNAME,l.LSNUMBER,l.L1PASS,l.PACCEPT,m.PSVALUE from hlt_supervisor_lumisections_v2 l, hlt_supervisor_scalar_map m where l.RUNNR=m.RUNNR and l.PSINDEX=m.PSINDEX and l.PATHNAME=m.PATHNAME and l.RUNNR=83037 order by l.LSNUMBER;
  //
  
  try{
    //    lumi::HLTScaler* h=0;
    m_session->open();
    cond::Connection con(m_constr,-1);
    con.connect(m_session);
    cond::CoralTransaction& transaction=con.coralTransaction();
    coral::AttributeList bindVariableList;
    bindVariableList.extend("runnumber",typeid(int));
    transaction.start(true); 
    int stopRun=startRun+numberOfRuns;
    std::cout<<"schema name "<<transaction.nominalSchema().schemaName()<<std::endl;
    std::set<std::string> listoftabs;
    listoftabs=transaction.nominalSchema().listTables();
    std::string tabname("HLT_SUPERVISOR_LUMISECTIONS_V2");
    std::string maptabname("HLT_SUPERVISOR_SCALAR_MAP");
    //for( std::set<std::string>::iterator it=listoftabs.begin(); it!=listoftabs.end();++it ){
    //  std::cout<<"tab: "<<*it<<std::endl;
    //}
    if( !transaction.nominalSchema().existsTable(tabname) ) throw cond::TransactionException("coral","table not found");

    for( int currentRun=startRun;currentRun<stopRun;++currentRun){
      bindVariableList["runnumber"].data<int>()=currentRun;
      int npath=0;
      coral::IQuery* query0 = transaction.nominalSchema().tableHandle(tabname).newQuery();
      coral::AttributeList nls;
      nls.extend("npath",typeid(unsigned int));
      query0->addToOutputList("count(distinct PATHNAME)","npath");
      query0->setCondition("RUNNR =:runnumber",bindVariableList);
      query0->defineOutput(nls);
      coral::ICursor& c=query0->execute();
      if( !c.next() ){
	std::cout<<"requested run "<<currentRun<<" doesn't exist, do nothing"<<std::endl;
	//edm::LuminosityBlockID lu(currentRun,1);
	//cond::Time_t current=(cond::Time_t)(lu.value());
	//h=new lumi::HLTScaler;
	//h->setHLTNULL();
	//result.push_back(std::make_pair<lumi::HLTScaler*,cond::Time_t>(h,current));
	//c.close();
	//delete query0;
	//do not do the second query, break to the next run
	continue;
      }else{
	npath=c.currentRow()["npath"].data<unsigned int>();
	std::cout<<"number of paths "<<npath<<std::endl;
	c.close();
	delete query0;
	if (npath==0) { 
	  std::cout<<"requested run is empty "<<currentRun<<" do nothing"<<std::endl;
	  continue;
	}
      }
      //queries per run
      coral::IQuery* query1 = transaction.nominalSchema().newQuery();
      query1->addToTableList(tabname,"l");
      query1->addToTableList(maptabname,"m");
      query1->addToOutputList("l.LSNUMBER","lsnumber");
      query1->addToOutputList("l.PATHNAME","pathname");
      query1->addToOutputList("l.L1PASS","hltinput");
      query1->addToOutputList("l.PACCEPT","hltratecounter");
      query1->addToOutputList("m.PSVALUE","prescale");
      query1->setCondition("l.RUNNR=m.RUNNR AND l.PSINDEX=m.PSINDEX AND l.PATHNAME=m.PATHNAME AND l.RUNNR =:runnumber",bindVariableList);
      query1->addToOrderList("l.LSNUMBER");
      query1->setRowCacheSize( 10692 );
      coral::ICursor& cursor1 = query1->execute();
      
      std::vector< lumi::HLTInfo > hltinfoPerLumi;
	hltinfoPerLumi.reserve(100);
	int currentLumiSection=1;
	int currentPath=1;
	while( cursor1.next() ){
	  const coral::AttributeList& row=cursor1.currentRow();
	  currentLumiSection=(int)row["lsnumber"].data<long long>();//updateLumiSection
	  std::cout<<"current run "<<currentRun<<std::endl;
	  std::cout<<"currentLumiSection "<<currentLumiSection<<std::endl;
	  std::cout<<"current path number"<<currentPath<<std::endl;
	  int hltinput=(int)row["hltinput"].data<long long>();
	  int hltratecounter=(int)row["hltratecounter"].data<long long>();
	  std::string pathname=row["pathname"].data<std::string>();
	  int prescale=(int)row["prescale"].data<long long>();
	  lumi::HLTInfo hltinfo(pathname,hltinput,hltratecounter,prescale);	  
	  hltinfoPerLumi.push_back( hltinfo );
	  if(currentPath==npath){
	    lumi::HLTScaler* h=new lumi::HLTScaler;
	    edm::LuminosityBlockID lu(currentRun,currentLumiSection);
	    h->setHLTData(lu,hltinfoPerLumi);
	    cond::Time_t current=(cond::Time_t)(lu.value());
	    result.push_back(std::make_pair<lumi::HLTScaler*,cond::Time_t>(h,current));
	    hltinfoPerLumi.clear();
	    currentPath=0;
	  }//end if it's last path in the current lumisection	 
	  ++currentPath;
	}
	cursor1.close();
	delete query1;
    }
    std::cout<<"commit transaction"<<std::endl;
    transaction.commit(); 
  }catch(const std::exception& er){
    std::cout<<"caught exception "<<er.what()<<std::endl;
    throw er;
  }
}

DEFINE_EDM_PLUGIN(lumi::HLTScalerReaderFactory,lumi::HLTScalerDBReader,"db");
