#include "tws_xml.h"

#include "debug.h"
#include "twsUtil.h"
#include "ibtws/Contract.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>





#define ADD_ATTR_INT( _struct_, _attr_ ) \
	if( !TwsXml::skip_defaults || _struct_._attr_ != dflt._attr_ ) { \
		snprintf(tmp, sizeof(tmp), "%d",_struct_._attr_ ); \
		xmlNewProp ( ne, (xmlChar*) #_attr_, (xmlChar*) tmp ); \
	}

#define ADD_ATTR_LONG( _struct_, _attr_ ) \
	if( !TwsXml::skip_defaults || _struct_._attr_ != dflt._attr_ ) { \
		snprintf(tmp, sizeof(tmp), "%ld",_struct_._attr_ ); \
		xmlNewProp ( ne, (xmlChar*) #_attr_, (xmlChar*) tmp ); \
	}

#define ADD_ATTR_DOUBLE( _struct_, _attr_ ) \
	if( !TwsXml::skip_defaults || _struct_._attr_ != dflt._attr_ ) { \
		snprintf(tmp, sizeof(tmp), "%.10g", _struct_._attr_ ); \
		xmlNewProp ( ne, (xmlChar*) #_attr_, (xmlChar*) tmp ); \
	}

#define ADD_ATTR_BOOL( _struct_, _attr_ ) \
	if( !TwsXml::skip_defaults || _struct_._attr_ != dflt._attr_ ) { \
		xmlNewProp ( ne, (xmlChar*) #_attr_, \
			(xmlChar*) (_struct_._attr_ ? "1" : "0") ); \
	}

#define ADD_ATTR_STRING( _struct_, _attr_ ) \
	if( !TwsXml::skip_defaults || _struct_._attr_ != dflt._attr_ ) { \
		xmlNewProp ( ne, (xmlChar*) #_attr_, \
			(xmlChar*) _struct_._attr_.c_str() ); \
	}


void conv_ib2xml( xmlNodePtr parent, const char* name, const IB::ComboLeg& cl )
{
	char tmp[128];
	static const IB::ComboLeg dflt;
	
	xmlNodePtr ne = xmlNewChild( parent, NULL, (const xmlChar*)name, NULL);
	
	ADD_ATTR_LONG( cl, conId );
	ADD_ATTR_LONG( cl, ratio );
	ADD_ATTR_STRING( cl, action );
	ADD_ATTR_STRING( cl, exchange );
	ADD_ATTR_LONG( cl, openClose );
	ADD_ATTR_LONG( cl, shortSaleSlot );
	ADD_ATTR_STRING( cl, designatedLocation );
	ADD_ATTR_INT( cl, exemptCode );
	
	xmlAddChild(parent, ne);
}


void conv_ib2xml( xmlNodePtr parent, const char* name, const IB::UnderComp& uc )
{
	char tmp[128];
	static const IB::UnderComp dflt;
	
	xmlNodePtr ne = xmlNewChild( parent, NULL, (const xmlChar*)name, NULL);
	
	ADD_ATTR_LONG( uc, conId );
	ADD_ATTR_DOUBLE( uc, delta );
	ADD_ATTR_DOUBLE( uc, price );
	
	xmlAddChild(parent, ne);
}


void conv_ib2xml( xmlNodePtr parent, const char* name, const IB::Contract& c )
{
	char tmp[128];
	static const IB::Contract dflt;
	
	xmlNodePtr ne = xmlNewChild( parent, NULL, (const xmlChar*)name, NULL);
	
	ADD_ATTR_LONG( c, conId );
	ADD_ATTR_STRING( c, symbol );
	ADD_ATTR_STRING( c, secType );
	ADD_ATTR_STRING( c, expiry );
	ADD_ATTR_DOUBLE( c, strike );
	ADD_ATTR_STRING( c, right );
	ADD_ATTR_STRING( c, multiplier );
	ADD_ATTR_STRING( c, exchange );
	ADD_ATTR_STRING( c, primaryExchange );
	ADD_ATTR_STRING( c, currency );
	ADD_ATTR_STRING( c, localSymbol );
	ADD_ATTR_BOOL( c, includeExpired );
	ADD_ATTR_STRING( c, secIdType );
	ADD_ATTR_STRING( c, secId );
	ADD_ATTR_STRING( c, comboLegsDescrip );
	
	if( c.comboLegs != NULL ) {
		xmlNodePtr ncl = xmlNewChild( ne, NULL, (xmlChar*)"comboLegs", NULL);
		
		IB::Contract::ComboLegList::const_iterator it = c.comboLegs->begin();
		for ( it = c.comboLegs->begin(); it != c.comboLegs->end(); ++it) {
			conv_ib2xml( ncl, "comboLeg", **it );
		}
	}
	if( c.underComp != NULL ) {
		conv_ib2xml( ne, "underComp", *c.underComp );
	}
	
	xmlAddChild(parent, ne);
}


void conv_ib2xml( xmlNodePtr parent, const char* name,
	const IB::ContractDetails& cd )
{
	char tmp[128];
	static const IB::ContractDetails dflt;
	
	xmlNodePtr ne = xmlNewChild( parent, NULL,
		(const xmlChar*)name, NULL);
	
	conv_ib2xml( ne, "summary", cd.summary );
	
	ADD_ATTR_STRING( cd, marketName );
	ADD_ATTR_STRING( cd, tradingClass );
	ADD_ATTR_DOUBLE( cd, minTick );
	ADD_ATTR_STRING( cd, orderTypes );
	ADD_ATTR_STRING( cd, validExchanges );
	ADD_ATTR_LONG( cd, priceMagnifier );
	ADD_ATTR_INT( cd, underConId );
	ADD_ATTR_STRING( cd, longName );
	ADD_ATTR_STRING( cd, contractMonth );
	ADD_ATTR_STRING( cd, industry );
	ADD_ATTR_STRING( cd, category );
	ADD_ATTR_STRING( cd, subcategory );
	ADD_ATTR_STRING( cd, timeZoneId );
	ADD_ATTR_STRING( cd, tradingHours );
	ADD_ATTR_STRING( cd, liquidHours );
	
	// BOND values
	ADD_ATTR_STRING( cd, cusip );
	ADD_ATTR_STRING( cd, ratings );
	ADD_ATTR_STRING( cd, descAppend );
	ADD_ATTR_STRING( cd, bondType );
	ADD_ATTR_STRING( cd, couponType );
	ADD_ATTR_BOOL( cd, callable );
	ADD_ATTR_BOOL( cd, putable );
	ADD_ATTR_DOUBLE( cd, coupon );
	ADD_ATTR_BOOL( cd, convertible );
	ADD_ATTR_STRING( cd, maturity );
	ADD_ATTR_STRING( cd, issueDate );
	ADD_ATTR_STRING( cd, nextOptionDate );
	ADD_ATTR_STRING( cd, nextOptionType );
	ADD_ATTR_BOOL( cd, nextOptionPartial );
	ADD_ATTR_STRING( cd, notes );
	
	xmlAddChild(parent, ne);
}



#define GET_ATTR_INT( _struct_, _attr_ ) \
	tmp = (char*) xmlGetProp( node, (xmlChar*) #_attr_ ); \
	_struct_->_attr_ = tmp ? atoi( tmp ) : dflt._attr_; \
	free(tmp)

#define GET_ATTR_LONG( _struct_, _attr_ ) \
	tmp = (char*) xmlGetProp( node, (xmlChar*) #_attr_ ); \
	_struct_->_attr_ = tmp ? atol( tmp ) : dflt._attr_; \
	free(tmp)

#define GET_ATTR_DOUBLE( _struct_, _attr_ ) \
	tmp = (char*) xmlGetProp( node, (xmlChar*) #_attr_ ); \
	_struct_->_attr_ = tmp ? atof( tmp ) : dflt._attr_; \
	free(tmp)

#define GET_ATTR_BOOL( _struct_, _attr_ ) \
	tmp = (char*) xmlGetProp( node, (xmlChar*) #_attr_ ); \
	_struct_->_attr_ = tmp ? atoi( tmp ) : dflt._attr_; \
	free(tmp)

#define GET_ATTR_STRING( _struct_, _attr_ ) \
	tmp = (char*) xmlGetProp( node, (xmlChar*) #_attr_ ); \
	_struct_->_attr_ = tmp ? std::string(tmp) : dflt._attr_; \
	free(tmp)


void conv_xml2ib( IB::ComboLeg* cl, const xmlNodePtr node )
{
	char* tmp;
	static const IB::ComboLeg dflt;

	GET_ATTR_LONG( cl, conId );
	GET_ATTR_LONG( cl, ratio );
	GET_ATTR_STRING( cl, action );
	GET_ATTR_STRING( cl, exchange );
	GET_ATTR_LONG( cl, openClose );
	GET_ATTR_LONG( cl, shortSaleSlot );
	GET_ATTR_STRING( cl, designatedLocation );
	GET_ATTR_INT( cl, exemptCode );
}

void conv_xml2ib( IB::UnderComp* uc, const xmlNodePtr node )
{
	char* tmp;
	static const IB::UnderComp dflt;
	
	GET_ATTR_LONG( uc, conId );
	GET_ATTR_DOUBLE( uc, delta );
	GET_ATTR_DOUBLE( uc, price );
}

void conv_xml2ib( IB::Contract* c, const xmlNodePtr node )
{
	char* tmp;
	static const IB::Contract dflt;
	
	GET_ATTR_LONG( c, conId );
	GET_ATTR_STRING( c, symbol );
	GET_ATTR_STRING( c, secType );
	GET_ATTR_STRING( c, expiry );
	GET_ATTR_DOUBLE( c, strike );
	GET_ATTR_STRING( c, right );
	GET_ATTR_STRING( c, multiplier );
	GET_ATTR_STRING( c, exchange );
	GET_ATTR_STRING( c, primaryExchange );
	GET_ATTR_STRING( c, currency );
	GET_ATTR_STRING( c, localSymbol );
	GET_ATTR_BOOL( c, includeExpired );
	GET_ATTR_STRING( c, secIdType );
	GET_ATTR_STRING( c, secId );
	GET_ATTR_STRING( c, comboLegsDescrip );
	
	for( xmlNodePtr p = node->children; p!= NULL; p=p->next) {
		if(p->name && (strcmp((char*) p->name, "comboLegs") == 0)) {
			if( c->comboLegs == NULL ) {
				c->comboLegs = new IB::Contract::ComboLegList();
			} else {
				c->comboLegs->clear();
			}
			for( xmlNodePtr q = p->children; q!= NULL; q=q->next) {
				IB::ComboLeg *cl = new IB::ComboLeg();
				conv_xml2ib( cl, q );
				c->comboLegs->push_back(cl);
			}
		} else if( p->name && (strcmp((char*) p->name, "underComp") == 0)) {
			if( c->underComp == NULL ) {
				c->underComp = new IB::UnderComp();
			}
			conv_xml2ib( c->underComp, p );
		}
		
	}
}


void conv_xml2ib( IB::ContractDetails* cd, const xmlNodePtr node )
{
	char* tmp;
	static const IB::ContractDetails dflt;
	
	xmlNodePtr xc = xmlFirstElementChild( node );
	conv_xml2ib( &cd->summary, xc );
	assert( strcmp((char*)xc->name,"summary") == 0 ); //TODO
	
	GET_ATTR_STRING( cd, marketName );
	GET_ATTR_STRING( cd, tradingClass );
	GET_ATTR_DOUBLE( cd, minTick );
	GET_ATTR_STRING( cd, orderTypes );
	GET_ATTR_STRING( cd, validExchanges );
	GET_ATTR_LONG( cd, priceMagnifier );
	GET_ATTR_INT( cd, underConId );
	GET_ATTR_STRING( cd, longName );
	GET_ATTR_STRING( cd, contractMonth );
	GET_ATTR_STRING( cd, industry );
	GET_ATTR_STRING( cd, category );
	GET_ATTR_STRING( cd, subcategory );
	GET_ATTR_STRING( cd, timeZoneId );
	GET_ATTR_STRING( cd, tradingHours );
	GET_ATTR_STRING( cd, liquidHours );
	
	// BOND values
	GET_ATTR_STRING( cd, cusip );
	GET_ATTR_STRING( cd, ratings );
	GET_ATTR_STRING( cd, descAppend );
	GET_ATTR_STRING( cd, bondType );
	GET_ATTR_STRING( cd, couponType );
	GET_ATTR_BOOL( cd, callable );
	GET_ATTR_BOOL( cd, putable );
	GET_ATTR_DOUBLE( cd, coupon );
	GET_ATTR_BOOL( cd, convertible );
	GET_ATTR_STRING( cd, maturity );
	GET_ATTR_STRING( cd, issueDate );
	GET_ATTR_STRING( cd, nextOptionDate );
	GET_ATTR_STRING( cd, nextOptionType );
	GET_ATTR_BOOL( cd, nextOptionPartial );
	GET_ATTR_STRING( cd, notes );
}




static int find_form_feed( const char *s, int n )
{
	int i;
	for( i=0; i<n; i++ ) {
		if( s[i] == '\f' ) {
			break;
		}
	}
	return i;
}

#define CHUNK_SIZE 1024
#define BUF_SIZE 1024 * 1024


TwsXml::TwsXml() :
	file(NULL),
	buf_size(0),
	buf(NULL),
	curDoc(NULL),
	curNode(NULL)
{
	resize_buf();
}

TwsXml::~TwsXml()
{
	if( file != NULL ) {
		fclose((FILE*)file);
	}
	free(buf);
	if( curDoc != NULL ) {
		xmlFreeDoc( curDoc );
	}
}

void TwsXml::resize_buf()
{
	buf_size = buf_size + BUF_SIZE;
	buf = (char*) realloc( buf, buf_size );
}

bool TwsXml::openFile( const char *filename )
{
	file = fopen(filename, "rb");
	
	if( file == NULL ) {
		fprintf( stderr, "error, %s: '%s'\n", strerror(errno), filename );
		return false;
	}
	
	return true;
}

xmlDocPtr TwsXml::nextXmlDoc()
{
	xmlDocPtr doc = NULL;
	if( file == NULL ) {
		return doc;
	}
	
	char *cp = buf;
	int tmp_len;
	while( (tmp_len = fread(cp, 1, CHUNK_SIZE, (FILE*)file)) > 0 ) {
		int ff = find_form_feed(cp, tmp_len);
		if( ff < tmp_len ) {
			int suc = fseek( (FILE*)file, -(tmp_len - (ff + 1)), SEEK_CUR);
			assert( suc == 0 );
			cp += ff;
			break;
		} else {
			cp += tmp_len;
			long cur_len = cp - buf;
			if( (cur_len + CHUNK_SIZE) >= buf_size ) {
				resize_buf();
				cp = buf + cur_len;
			}
		}
	}
	
	doc = xmlReadMemory( buf, cp-buf, "URL", NULL, 0 );
	
	return doc;
}

xmlNodePtr TwsXml::nextXmlRoot()
{
	if( curDoc != NULL ) {
		xmlFreeDoc(curDoc);
	}
	
	while( (curDoc = nextXmlDoc()) != NULL ) {
		xmlNodePtr root = xmlDocGetRootElement(curDoc);
		if( root != NULL ) {
			assert( root->type == XML_ELEMENT_NODE );
			if( strcmp((char*)root->name, "TWSXML") == 0 ) {
// 				fprintf(stderr, "Notice, return root '%s'.\n", root->name);
				return root;
			} else {
				fprintf(stderr, "Warning, ignore unknown root '%s'.\n",
					root->name);
			}
		} else {
			fprintf(stderr, "Warning, no root element found.\n");
		}
		xmlFreeDoc(curDoc);
	}
// 	fprintf(stderr, "Notice, all roots parsed.\n");
	return NULL;
}

xmlNodePtr TwsXml::nextXmlNode()
{
	if( curNode!= NULL ) {
		curNode = curNode->next;
	}
	for( ; curNode!= NULL; curNode = curNode->next ) {
		if( curNode->type == XML_ELEMENT_NODE ) {
// 			fprintf(stderr, "Notice, return element '%s'.\n", curNode->name);
			return curNode;
		} else {
// 			fprintf(stderr, "Warning, ignore element '%s'.\n", curNode->name);
		}
	}	
	assert( curNode == NULL );
	
	xmlNodePtr root;
	while( (root = nextXmlRoot()) != NULL ) {
		for( xmlNodePtr p = root->children; p!= NULL; p=p->next) {
			if( p->type == XML_ELEMENT_NODE ) {
				curNode = p;
				break;
			} else {
// 				fprintf(stderr, "Warning, ignore element '%s'.\n", p->name);
			}
		}
		if( curNode != NULL ) {
			break;
		} else {
			fprintf(stderr, "Warning, no usable element found.\n");
		}
	}
	
	if( curNode != NULL ) {
// 		fprintf(stderr, "Notice, return element '%s'.\n", curNode->name);
	} else {
// 		fprintf(stderr, "Notice, all elements parsed.\n");
	}
	
	return curNode;
}




bool TwsXml::_skip_defaults = false;
const bool& TwsXml::skip_defaults = _skip_defaults;

void TwsXml::setSkipDefaults( bool b )
{
	_skip_defaults = b;
}

xmlNodePtr TwsXml::newDocRoot()
{
	xmlDocPtr doc = xmlNewDoc( (const xmlChar*) "1.0");
	xmlNodePtr root = xmlNewDocNode( doc, NULL,
		(const xmlChar*)"TWSXML", NULL );
	xmlDocSetRootElement( doc, root );
	
	//caller has to free root.doc
	return root;
}

void TwsXml::dumpAndFree( xmlNodePtr root )
{
	xmlDocFormatDump(stdout, root->doc, 1);
	//HACK print form feed as xml file separator
	printf("\f");
	
	xmlFreeDoc(root->doc);
}
