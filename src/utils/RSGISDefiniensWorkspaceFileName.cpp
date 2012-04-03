/*
 *  RSGISDefiniensWorkspaceFileName.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/02/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
 * 
 *  RSGISLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RSGISLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISDefiniensWorkspaceFileName.h"

namespace rsgis{namespace utils{
	
	RSGISDefiniensWorkspaceFileName::RSGISDefiniensWorkspaceFileName(string filename) 
	{
		this->parseFileName(filename);
		this->oldVersion = false;
	}
	
	const string RSGISDefiniensWorkspaceFileName::getFileName() const
	{
		RSGISMathsUtils mathsUtils;
		string filename = start + string(".tile") + tileNumStr + string(".v") + mathsUtils.inttostring(version) + extension;
		return filename;
	}
	
	string RSGISDefiniensWorkspaceFileName::getFileNameWithPath()
	{
		RSGISMathsUtils mathsUtils;
		string filename = path + start + string(".tile") + tileNumStr + string(".v") + mathsUtils.inttostring(version) + extension;
		return filename;
	}
	
	string RSGISDefiniensWorkspaceFileName::getFileNameNoExtension()
	{
		RSGISMathsUtils mathsUtils;
		string filename = start + string(".tile") + tileNumStr + string(".v") + mathsUtils.inttostring(version);
		return filename;
	}
	
	int RSGISDefiniensWorkspaceFileName::getTile()
	{
		return this->tile;
	}
	
	int RSGISDefiniensWorkspaceFileName::getVersion()
	{
		return this->version;
	}
	
	string RSGISDefiniensWorkspaceFileName::getBaseName()
	{
		return this->start;
	}
	
	void RSGISDefiniensWorkspaceFileName::setOldVersion(bool oldVersion)
	{
		this->oldVersion = oldVersion;
	}
	
	bool RSGISDefiniensWorkspaceFileName::getOldVersion() const
	{
		return this->oldVersion;
	}
	
	ostream& operator<<(ostream& ostr, const RSGISDefiniensWorkspaceFileName& name)
	{
		ostr << name.getFileName();
		if(name.getOldVersion())
		{
			ostr << "(OLD VERSION)";
		}
		return ostr;
	}
	
	ostream& RSGISDefiniensWorkspaceFileName::operator<<(ostream& ostr)
	{
		ostr << start << ".tile" << tile << ".v" << version << "." << extension;
		if(oldVersion)
		{
			ostr << "(OLD VERSION)";
		}
		return ostr;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator==(RSGISDefiniensWorkspaceFileName name) const
	{
		if(start != name.start)
		{
			return false;
		}
		else if(extension != name.extension)
		{
			return false;
		}
		else if(tile != name.tile)
		{
			return false;
		}
		else if(version != name.version)
		{
			return false;
		}
		
		return true;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator!=(RSGISDefiniensWorkspaceFileName name) const
	{
		if(start != name.start)
		{
			return true;
		}
		else if(extension != name.extension)
		{
			return true;
		}
		else if(tile != name.tile)
		{
			return true;
		}
		else if(version != name.version)
		{
			return true;
		}
		
		return false;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator>(RSGISDefiniensWorkspaceFileName name) const
	{
		if(tile > name.tile)
		{
			return true;
		}
		else if(tile == name.tile & version > name.version)
		{
			return true;
		}
		
		return false;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator<(RSGISDefiniensWorkspaceFileName name) const
	{
		if(tile < name.tile)
		{
			return true;
		}
		else if(tile == name.tile & version < name.version)
		{
			return true;
		}
		
		return false;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator>=(RSGISDefiniensWorkspaceFileName name) const
	{
		if(tile == name.tile & version == name.version)
		{
		   return true;
		}
		else if(tile > name.tile)
		{
			return true;
		}
		else if(tile == name.tile & version > name.version)
		{
			return true;
		}
		return false;
	}
	
	bool RSGISDefiniensWorkspaceFileName::operator<=(RSGISDefiniensWorkspaceFileName name) const
	{
		if(tile == name.tile & version == name.version)
		{
			return true;
		}
		else if(tile < name.tile)
		{
			return true;
		}
		else if(tile == name.tile & version < name.version)
		{
			return true;
		}
		
		return false;
	}
	
	void RSGISDefiniensWorkspaceFileName::parseFileName(string filename)
	{
		RSGISFileUtils fileUtils;
		RSGISMathsUtils mathsUtils;
		//cout << "Parsing filename: " << filename << endl;
		this->path = fileUtils.getFileDirectoryPath(filename);
		//cout << "Path: " << this->path << endl;
		this->extension = fileUtils.getExtension(filename);
		//cout << "Extension: \'" << this->extension << "\'" << endl;
		string filenameNoExtension = fileUtils.getFileNameNoExtension(filename);
		//cout << "File Name no extension: " << filenameNoExtension << endl;
		string versionStr = fileUtils.getExtension(filenameNoExtension);
		//cout << "Version (String): " << versionStr << endl;
		string versionNumStr = versionStr.substr(2);
		//cout << "Version Number Only (as string): \'" << versionNumStr << "\'" << endl;
		this->version = mathsUtils.strtoint(versionNumStr);
		//cout << "Version: " << this->version << endl;
		string filenameNoVersion = fileUtils.getFileNameNoExtension(filenameNoExtension);
		string tileStr = fileUtils.getExtension(filenameNoVersion);
		//cout << "Tile (String): " << tileStr << endl;
		this->tileNumStr = tileStr.substr(5);
		//cout << "Tile Number Only (as string): \'" << tileNumStr << "\'" << endl;
		this->tile = mathsUtils.strtoint(tileNumStr);
		//cout << "tile: " << this->tile << endl;
		this->start = fileUtils.getFileNameNoExtension(filenameNoVersion);
		//cout << "Start: " << this->start << endl;
	}
	
	RSGISDefiniensWorkspaceFileName::~RSGISDefiniensWorkspaceFileName()
	{
		
	}
	
}} //rsgis::utils


