//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2020   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see LICENSE);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//=========================================================================//


#include "chipcollection.h"

#include "globals.h"
#include "defines.h"

#include <QVector>
#include <QString>
#include <QDebug>
#include <QXmlReader>
#include <QDomNode>


// will be deprecated
quint32 cChipCollection::GetSubType(quint32 type)
{
	return (quint32)(type & 0x7FFF);
}

quint32 cChipCollection::GetPriType(quint32 type)
{
	return (int)((type >> 16) & 0x7F);
}

cGroupElement *cChipCollection::GetMenuGroupPointer(const QString &menuStr)
{
	cGroupElement *grE = NULL;

	foreach (cGroupElement *g, icGroups)
	{
		if (menuStr == g->menuName)
		{
			grE = g;
			break;
		}
	}

	if (grE == NULL)
	{
		grE = new cGroupElement();
		//grE->menuName = menuStr;
	}

	return grE;
}

quint32 cChipCollection::GetTypeFromSize(quint32 type, int size)
{
	foreach (cGroupElement *g, icGroups)
	{
		if (g->vId.indexOf((type >> 16) & 0x7f) == -1)
		{
			continue;
		}

		foreach (icElement iE, g->vChip)
		{
			int sz = 0;
			if (iE.prog_sz > 0)
			{
				sz = iE.prog_sz;
			}
			if (iE.data_sz > 0)
			{
				sz += iE.data_sz;
			}

			if (sz == size)
			{
				return iE.id;
			}
		}
	}

	return -1;
}


int cChipCollection::GetTypeSize(quint32 type)
{
	icElement *i = getChipPointer(type);

	int sz = 0;
	if (i->prog_sz > 0)
	{
		sz = i->prog_sz;
	}

	if (i->data_sz > 0)
	{
		sz += i->data_sz;
	}

	return sz;
}


int cChipCollection::GetAddrSize(quint32 type)
{
	icElement *i = getChipPointer(type);

	return i->adr_sz;
}


int cChipCollection::GetTypeSplit(quint32 type)
{
	icElement *i = getChipPointer(type);

	return i->prog_sz;
}


int cChipCollection::GetTypeWPageSize(quint32 type)
{
	icElement *i = getChipPointer(type);

	return i->wpg_sz;
}


QString cChipCollection::GetTypeString(quint32 type)
{
	foreach (cGroupElement *g, icGroups)
	{
		if (g->vId.indexOf((type >> 16) & 0x7f) == -1)
		{
			continue;
		}

		foreach (icElement i, g->vChip)
		{
			if (i.id == type)
			{
				return i.name;
			}
		}
	}

	return "";
}

quint32 cChipCollection::GetSignatureType(quint32 pri_type, quint16 sign)
{
	foreach (cGroupElement *g, icGroups)
	{
		if (g->vId.indexOf(pri_type) == -1)
		{
			continue;
		}

		foreach (icElement i, g->vChip)
		{
			foreach (quint16 s, i.sign)
			{
				if (s == sign)
				{
					return i.id;
				}
			}
		}
	}

	return EID_INVALID;
}

quint32 cChipCollection::GetFirstFromPritype(quint32 pritype)
{
	foreach (cGroupElement *g, icGroups)
	{
		if (g->vId.indexOf(pritype) == -1)
		{
			continue;
		}

		foreach (icElement i, g->vChip)
		{
			if (((i.id >> 16) & 0x7f) == pritype)
			{
				return i.id;
			}
		}
	}

	return EID_INVALID;
}

/**
 * @brief search chip name in vectors
 *
 */
quint32 cChipCollection::GetTypeFromString(const QString &name)
{
	foreach (cGroupElement *g, icGroups)
	{
		foreach (icElement i, g->vChip)
		{
			if (i.name == name)
			{
				return i.id;
			}
		}
	}

	return EID_INVALID;
}

/**
 * @brief funktion generates list of menu/submenue elements of main program
 */
QVector <menuToGroup> cChipCollection::CreateDeviceMenues()
{
	// the loop about all structures
	qDebug() << Q_FUNC_INFO << icGroups.count() << "entries";

	QVector <menuToGroup> m;

	foreach (cGroupElement *g, icGroups)
	{
		menuToGroup *mTmp = new menuToGroup();
		mTmp->title = g->menuName;
		mTmp->vId = g->vId; // vector of group ids

		foreach (icElement iE, g->vChip)
		{
			chipMenuInfo cInfo = (chipMenuInfo)
			{
				iE.name, iE.id
			};

			mTmp->vChip << cInfo;
		}

		m << *mTmp;
	}

	return m;
}


quint32 cChipCollection::GetCurrentId()
{
	return eep_id;
}

void cChipCollection::SetCurrentId(quint32 id)
{
	if (id == 0)
	{
		id = E2400;         //to avoid segV
	}

	eep_id = id;

}

/**
 * part for parsing of fuse/lock text nodes
 *
 */
bool cChipCollection::parseDescrNode(const QDomNode &nd, QVector<BitInfo> &b, QVector<MaskDescr> &d)
{
	QDomNodeList intNodes = nd.childNodes();

	for (int l = 0; l < intNodes.count(); l++)
	{
		if (intNodes.at(l).nodeName() == "bit")
		{
			QDomElement bitInfo = intNodes.at(l).toElement();

			//children "bit" with attributes "offset", "name", "index" -> struct BitInfo
			QString offset = bitInfo.attribute("offset");
			QString name = bitInfo.attribute("name");
			QString index = bitInfo.attribute("index");
			QString description = bitInfo.attribute("text");

			BitInfo *bhelp = new BitInfo;
			bhelp->bit = offset.toInt();
			bhelp->idx = index.toInt();
			bhelp->ShortDescr = name + index;
			bhelp->LongDescr = description;

			b << *bhelp;
		}

		if (intNodes.at(l).nodeName() == "set")
		{
			QDomElement setInfo = intNodes.at(l).toElement();

			//children "set" with attributes "code" "text" and optional "additional" -> struct MaskDescr
			QString code = setInfo.attribute("code");
			QString text = setInfo.attribute("text");
			QString additional = setInfo.attribute("additional", "");

			MaskDescr *mHelp = new MaskDescr;
			mHelp->mask = code;
			mHelp->LongDescr = text;
			mHelp->ExtDescr = additional;

			d << *mHelp;
		}
	}

	return true;
}


bool cChipCollection::ReadConfigFromXml(const QString &filename)
{
	QDomDocument doc;
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
	{
		return false;
	}

	bool err = false;
	qDebug() << "read from config" << filename;

	QDomNodeList ics = doc.elementsByTagName("chip");

	for (int i = 0; i < ics.size(); i++)
	{
		QDomNode n = ics.item(i);

		QString menuStr = n.attributes().namedItem("menu").nodeValue();
		QString idStr = n.attributes().namedItem("id").nodeValue();

		qDebug() << menuStr << idStr;

		int pre_nr = idStr.toInt(NULL, 16);

		bool newGroupElem = false;

		cGroupElement *grE = GetMenuGroupPointer(menuStr);

		if (grE->menuName == "")
		{
			grE->menuName = menuStr;
			newGroupElem = true;
		}

		grE->vId.push_back(pre_nr);

		QDomNodeList icList = n.childNodes();

		icElement *iE = NULL;
		int run_id = 0;

		for (int j = 0; j < icList.size(); j++)
		{
			QDomElement memInfo = icList.at(j).toElement();

			if (memInfo.isNull())
			{
				continue;
			}

			if (memInfo.nodeName() == "ic")
			{
				if (!memInfo.hasAttribute("menu"))
				{
					err = true;
					qDebug() << "attribute 'menu' was not found";
					break;
				}

				run_id++;

				QString mName = memInfo.attribute("menu");
				QString boot_addr = memInfo.attribute("boot", "0");
				QString sgn = memInfo.attribute("signature", "");
				QStringList sgn_list = sgn.split(",");
				QString code_sz = "-1";
				QString dat_sz = "-1";
				QString adr_sz = "-1";
				QString wpg_sz = "-1";
				QString rpg_sz = "-1";
				QString fuse_mask = "-1";
				QString fuse_rcmd = "-1";
				QString fuse_wcmd = "-1";
				QString lock_mask = "-1";
				QString lock_rcmd = "-1";
				QString lock_wcmd = "-1";

				qDebug() << mName;

				QDomNodeList chip_nodes = memInfo.childNodes();

				for (int k = 0; k < chip_nodes.size(); k++)
				{
					QDomNode n_chip = chip_nodes.item(k);
					if (n_chip.isNull())
					{
						continue;
					}

					qDebug() << n_chip.nodeName();

					if (n_chip.nodeName() == "size")
					{
						code_sz = n_chip.attributes().namedItem("code").nodeValue();
						dat_sz = n_chip.attributes().namedItem("data").nodeValue();
						adr_sz = n_chip.attributes().namedItem("adr").nodeValue();
						wpg_sz = n_chip.attributes().namedItem("wpg").nodeValue();
						rpg_sz = n_chip.attributes().namedItem("rpg").nodeValue();
					}

					if (n_chip.nodeName() == "fuse")
					{
						fuse_mask = n_chip.attributes().namedItem("mask").nodeValue();
						fuse_wcmd = n_chip.attributes().namedItem("write_cmd").nodeValue();
						fuse_rcmd = n_chip.attributes().namedItem("read_cmd").nodeValue();
					}

					if (n_chip.nodeName() == "lock")
					{
						lock_mask = n_chip.attributes().namedItem("mask").nodeValue();
						lock_wcmd = n_chip.attributes().namedItem("write_cmd").nodeValue();
						lock_rcmd = n_chip.attributes().namedItem("read_cmd").nodeValue();
					}
				}

				iE = new icElement;

				iE->name = mName;
				if (sgn != "")
				{
					foreach (QString s, sgn_list)
					{
						iE->sign << convertSize(s);
					}
				}

				iE->prog_sz = convertSize(code_sz);
				iE->data_sz = convertSize(dat_sz);
				iE->wpg_sz = convertSize(wpg_sz);
				iE->adr_sz = convertSize(adr_sz);
				iE->rpg_sz = convertSize(rpg_sz);
				iE->boot = convertSize(boot_addr);

				iE->id = (pre_nr << 16) + run_id;

				grE->vChip.push_back(*iE);
			}

			if (memInfo.nodeName() == "descr")
			{
				QDomElement helpInfo = memInfo.toElement();//.firstChildElement("descr");
				run_id = 0;

				QString names = helpInfo.attributes().namedItem("list").nodeValue();

				qDebug() << names;

				QStringList nList = names.split(QRegExp(",\\s*"));

				chipBits bStruct;

				bStruct.chNames = nList;

				//TODO check names

				QDomNodeList icLock = memInfo.childNodes();

				for (int k = 0; k < icLock.size(); k++)
				{
					QDomNode n = icLock.item(k);
					if (n.isNull())
					{
						continue;
					}

					if (n.nodeName() == "lock")
					{
						parseDescrNode(n, bStruct.lock, bStruct.lockDescr);
					}

					if (n.nodeName() == "fuse")
					{
						parseDescrNode(n, bStruct.fuse, bStruct.fuseDescr);
					}
				}

				grE->helper << bStruct;
			}
		}

		if (newGroupElem)
		{
			//groupList << *grE;
			addGroup(grE);
		}
	}

	file.close();

	return !err;
}

chipBits *cChipCollection::GetTypeFuses(quint32 type)
{
	chipBits *cb = NULL;
	quint16 pri_type = ((type >> 16) & 0x07f);
	foreach (cGroupElement *g, icGroups)
	{
		if (g->vId.indexOf(pri_type) == -1)
		{
			continue;
		}

		QString name = "";

		foreach (icElement i, g->vChip)
		{
			if (i.id == type)
			{
				name = i.name;
			}
		}

		if (name == "")
		{
			return NULL;
		}

		foreach (chipBits c, g->helper)
		{
			int n = c.chNames.indexOf(name);
			if (n >= 0)
			{
				cb = new chipBits;
				*cb = c;
				return cb;
			}
		}
	}

	return NULL;
}


bool cChipCollection::addGroup(cGroupElement *g)
{
	// TODO: check the structures
	icGroups << g;
	return true;
}


icElement *cChipCollection::getChipPointer(quint32 type)
{
	icElement *info = NULL;

	foreach (cGroupElement *g, icGroups)
	{
		if (g->vId.indexOf((type >> 16) & 0x7f) == -1)
		{
			continue;
		}

		foreach (icElement iE, g->vChip)
		{
			// extract only subtype
			if (iE.id == type)
			{
				info = &iE;
				break;
			}
		}
	}

	return info;
}


/**
* convert string 1k, 2k ... to int
* convert hex numbers to int
*/
int cChipCollection::convertSize(const QString &s)
{
	int res = 0;
	bool cnv = true;

	if (s.indexOf(QRegExp("[0-9]+k")) >= 0)
	{
		QString s_tmp = s;
		s_tmp.remove("k");
		res = s_tmp.toInt(&cnv);
		res *= 1024;
	}
	else if (s.indexOf(QRegExp("0x[0-9a-fA-F]+")) >= 0)
	{
		res = s.toInt(&cnv, 16);
	}
	else
	{
		res = s.toInt(&cnv);
	}

	if (cnv == false)
	{
		res = -1;
	}

	return res;
}

