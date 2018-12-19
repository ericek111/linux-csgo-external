#include "netvar.h"

vector<netvar::class_t> g_classes;

netvar::class_t read_class(remote::Handle csgo, remote::MapModuleMemoryRegion client, netvar::ClientClass cls) {
	netvar::class_t ret;

	netvar::RecvTable table;

	if(csgo.Read(cls.m_pRecvTable, &table, sizeof(table))) {
		netvar::RecvProp* props = new netvar::RecvProp[table.m_nProps];

		if(csgo.Read(table.m_pProps, props, sizeof(netvar::RecvProp) * table.m_nProps)) {
			for(size_t i = 0; i < table.m_nProps; i++) {
				char propName[256];

				if(csgo.Read(props[i].m_pVarName, propName, 256)) {
					netvar::prop_t p;

					p.name = propName;
					p.offset = (size_t) props[i].m_Offset;

					ret.props.push_back(p);
				}
			}
		}

		delete[] props;
	}

	char networkName[256];

	if(csgo.Read((void*) cls.m_pNetworkName, networkName, 256)) {
		ret.name = networkName;
	}

	return ret;
}

void netvar::DumpTable(remote::Handle csgo, remote::MapModuleMemoryRegion client, void* tableptr, int depth, stringstream& out) {
	netvar::RecvTable table;

	if(csgo.Read(tableptr, &table, sizeof(table))) {
		netvar::RecvProp* props = new netvar::RecvProp[table.m_nProps];

		if(csgo.Read(table.m_pProps, props, sizeof(netvar::RecvProp) * table.m_nProps)) {
			for(size_t i = 0; i < table.m_nProps; i++) {
				char propName[256];

				if(csgo.Read(props[i].m_pVarName, propName, 256)) {
					netvar::prop_t p;

					p.name = propName;
					p.offset = (size_t) props[i].m_Offset;

					string varName = string (propName);
					if (varName.find("baseclass") == 0 || varName.find("0") == 0 || varName.find("1") == 0 || varName.find("2") == 0)
						continue;
				
					for (int i = 0; i < depth; i++)
						out << "\t";
				
					out << "\t" << varName << " [0x" << std::hex << props[i].m_Offset << "]\n";

					if (props[i].m_pDataTable)
						DumpTable(csgo, client, props[i].m_pDataTable, depth + 1, out);
				}
			}
		}

		delete[] props;
	}
}

string netvar::DumpAll(remote::Handle csgo, remote::MapModuleMemoryRegion client) {
	ClientClass currentClass;
	stringstream out;

	if(!csgo.Read((void*) csgo.m_addressOfClientClassHead, &currentClass, sizeof(ClientClass))) {
		cout << "Error reading ClientClassHead" << endl;
		return string("");
	}

	while(currentClass.m_pNext) {
		if(!currentClass.m_pNetworkName)
			break;

		char networkName[256];
		if(csgo.Read((void*) currentClass.m_pNetworkName, networkName, sizeof(networkName))) {
			out << networkName << " (" << dec << currentClass.m_ClassID << "):\n";
		}

		netvar::DumpTable(csgo, client, currentClass.m_pRecvTable, 0, out);
		if(!csgo.Read(currentClass.m_pNext, &currentClass, sizeof(ClientClass))) {
			break;
		}
	}
	return out.str();
}

bool netvar::Cache(remote::Handle csgo, remote::MapModuleMemoryRegion client) {
	g_classes.clear();
	ClientClass currentClass;

	if(!csgo.Read((void*) csgo.m_addressOfClientClassHead, &currentClass, sizeof(ClientClass))) {
		cout << "Error reading ClientClassHead" << endl;
		return false;
	}

	while(currentClass.m_pNext) {
		if(!currentClass.m_pNetworkName)
			break;

		netvar::class_t cls = read_class(csgo, client, currentClass);

		if(cls.name.length() && cls.props.size() > 0) {
			g_classes.push_back(cls);
		}

		if(!csgo.Read(currentClass.m_pNext, &currentClass, sizeof(ClientClass))) {
			break;
		}
	}

	return (g_classes.size() > 0);
}

std::vector<netvar::class_t> netvar::GetAllClasses() {
	return g_classes;
}

ptrdiff_t netvar::GetOffset(std::string classname, std::string varname) {
	for(size_t i = 0; i < g_classes.size(); i++) {
		if(g_classes[i].name.compare(classname) != 0)
			continue;

		for(size_t p = 0; p < g_classes[i].props.size(); p++) {
			if(g_classes[i].props[p].name.compare(varname) == 0) {
				return g_classes[i].props[p].offset;
			}
		}
	}

	return -1;
}