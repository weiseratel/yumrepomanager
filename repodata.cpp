/*
    YUMRepoManager, a manager for the configuration files of YUM and his repositories.
    Copyright (C) 2011-2012  Josué V. Herrera

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "repodata.h"

RepoData::RepoData()
{

    repoFileName = "";
    repoName = "";

    repoComment[0] = "";
    repoComment[1] = "";

    repoFailoverMethod[0] = "";
    repoFailoverMethod[1] = "";
    repoFailoverMethod[2] = "";

    repoSkipIfUnavailable[0] = "";
    repoSkipIfUnavailable[1] = "";
    repoSkipIfUnavailable[2] = "";

    repoBaseURL[0] = "";
    repoBaseURL[1] = "";
    repoBaseURL[2] = "";

    repoMetaLink[0] = "";
    repoMetaLink[1] = "";
    repoMetaLink[2] = "";

    repoProxy[0] = "";
    repoProxy[1] = "";
    repoProxy[2] = "";

    repoProxyUsername[0] = "";
    repoProxyUsername[1] = "";
    repoProxyUsername[2] = "";

    repoProxyPassword[0] = "";
    repoProxyPassword[1] = "";
    repoProxyPassword[2] = "";

    repoBandwidth[0] = "";
    repoBandwidth[1] = "";
    repoBandwidth[2] = "";

    repoInclude[0] = "";
    repoInclude[1] = "";
    repoInclude[2] = "";

    repoExclude[0] = "";
    repoExclude[1] = "";
    repoExclude[2] = "";

    repoEnabled[0] = "";
    repoEnabled[1] = "";

    repoMetadataExpire[0] = "";
    repoMetadataExpire[1] = "";
    repoMetadataExpire[2] = "";

    repoGPGCheck[0] = "";
    repoGPGCheck[1] = "";
    repoGPGCheck[2] = "";

    repoGPGKey[0] = "";
    repoGPGKey[1] = "";
    repoGPGKey[2] = "";

    repoParallelDownload[0] = "";
    repoParallelDownload[1] = "";
    repoParallelDownload[2] = "";

}// Constructor

bool RepoData::operator ==(const RepoData &repoDataRight) {

    if(repoFileName != repoDataRight.repoFileName) {

        return false;

    }// if

    if(repoName != repoDataRight.repoName) {

        return false;

    }// if

    if(repoComment[0] != repoDataRight.repoComment[0]) {

        return false;

    }// if

    if(repoComment[1] != repoDataRight.repoComment[1]) {

        return false;

    }// if

    if(repoEnabled[0] != repoDataRight.repoEnabled[0]) {

        return false;

    }// if

    if(repoEnabled[1] != repoDataRight.repoEnabled[1]) {

        return false;

    }// if

    if(repoBaseURL[0] != repoDataRight.repoBaseURL[0]) {

        return false;

    }// if

    if(repoBaseURL[1] != repoDataRight.repoBaseURL[1]) {

        return false;

    }// if

    if(repoBaseURL[2] != repoDataRight.repoBaseURL[2]) {

        return false;

    }// if

    if(repoMetaLink[0] != repoDataRight.repoMetaLink[0]) {

        return false;

    }// if

    if(repoMetaLink[1] != repoDataRight.repoMetaLink[1]) {

        return false;

    }// if

    if(repoMetaLink[2] != repoDataRight.repoMetaLink[2]) {

        return false;

    }// if

    if(repoGPGCheck[0] != repoDataRight.repoGPGCheck[0]) {

        return false;

    }// if

    if(repoGPGCheck[1] != repoDataRight.repoGPGCheck[1]) {

        return false;

    }// if

    if(repoGPGCheck[2] != repoDataRight.repoGPGCheck[2]) {

        return false;

    }// if

    if(repoGPGKey[0] != repoDataRight.repoGPGKey[0]) {

        return false;

    }// if

    if(repoGPGKey[1] != repoDataRight.repoGPGKey[1]) {

        return false;

    }// if

    if(repoGPGKey[2] != repoDataRight.repoGPGKey[2]) {

        return false;

    }// if

    if(repoProxy[0] != repoDataRight.repoProxy[0]) {

        return false;

    }// if

    if(repoProxy[1] != repoDataRight.repoProxy[1]) {

        return false;

    }// if

    if(repoProxy[2] != repoDataRight.repoProxy[2]) {

        return false;

    }// if

    if(repoProxyUsername[0] != repoDataRight.repoProxyUsername[0]) {

        return false;

    }// if

    if(repoProxyUsername[1] != repoDataRight.repoProxyUsername[1]) {

        return false;

    }// if

    if(repoProxyUsername[2] != repoDataRight.repoProxyUsername[2]) {

        return false;

    }// if

    if(repoProxyPassword[0] != repoDataRight.repoProxyPassword[0]) {

        return false;

    }// if

    if(repoProxyPassword[1] != repoDataRight.repoProxyPassword[1]) {

        return false;

    }// if

    if(repoProxyPassword[2] != repoDataRight.repoProxyPassword[2]) {

        return false;

    }// if

    if(repoBandwidth[0] != repoDataRight.repoBandwidth[0]) {

        return false;

    }// if

    if(repoBandwidth[1] != repoDataRight.repoBandwidth[1]) {

        return false;

    }// if

    if(repoBandwidth[2] != repoDataRight.repoBandwidth[2]) {

        return false;

    }// if

    if(repoSkipIfUnavailable[0] != repoDataRight.repoSkipIfUnavailable[0]) {

        return false;

    }// if

    if(repoSkipIfUnavailable[1] != repoDataRight.repoSkipIfUnavailable[1]) {

        return false;

    }// if

    if(repoSkipIfUnavailable[2] != repoDataRight.repoSkipIfUnavailable[2]) {

        return false;

    }// if

    if(repoFailoverMethod[0] != repoDataRight.repoFailoverMethod[0]) {

        return false;

    }// if

    if(repoFailoverMethod[1] != repoDataRight.repoFailoverMethod[1]) {

        return false;

    }// if

    if(repoFailoverMethod[2] != repoDataRight.repoFailoverMethod[2]) {

        return false;

    }// if

    if(repoMetadataExpire[0] != repoDataRight.repoMetadataExpire[0]) {

        return false;

    }// if

    if(repoMetadataExpire[1] != repoDataRight.repoMetadataExpire[1]) {

        return false;

    }// if

    if(repoMetadataExpire[2] != repoDataRight.repoMetadataExpire[2]) {

        return false;

    }// if

    if(repoParallelDownload[0] != repoDataRight.repoParallelDownload[0]) {

        return false;

    }// if

    if(repoParallelDownload[1] != repoDataRight.repoParallelDownload[1]) {

        return false;

    }// if

    if(repoParallelDownload[2] != repoDataRight.repoParallelDownload[2]) {

        return false;

    }// if

    if(repoInclude[0] != repoDataRight.repoInclude[0]) {

        return false;

    }// if

    if(repoInclude[1] != repoDataRight.repoInclude[1]) {

        return false;

    }// if

    if(repoInclude[2] != repoDataRight.repoInclude[2]) {

        return false;

    }// if

    if(repoExclude[0] != repoDataRight.repoExclude[0]) {

        return false;

    }// if

    if(repoExclude[1] != repoDataRight.repoExclude[1]) {

        return false;

    }// if

    if(repoExclude[2] != repoDataRight.repoExclude[2]) {

        return false;

    }// if

    return true;

}// operator ==

bool RepoData::operator !=(const RepoData &repoDataRight) {

    if(repoFileName != repoDataRight.repoFileName) {

        return true;

    }// if

    if(repoName != repoDataRight.repoName) {

        return true;

    }// if

    if(repoComment[0] != repoDataRight.repoComment[0]) {

        return true;

    }// if

    if(repoComment[1] != repoDataRight.repoComment[1]) {

        return true;

    }// if

    if(repoEnabled[0] != repoDataRight.repoEnabled[0]) {

        return true;

    }// if

    if(repoEnabled[1] != repoDataRight.repoEnabled[1]) {

        return true;

    }// if

    if(repoBaseURL[0] != repoDataRight.repoBaseURL[0]) {

        return true;

    }// if

    if(repoBaseURL[1] != repoDataRight.repoBaseURL[1]) {

        return true;

    }// if

    if(repoBaseURL[2] != repoDataRight.repoBaseURL[2]) {

        return true;

    }// if

    if(repoMetaLink[0] != repoDataRight.repoMetaLink[0]) {

        return true;

    }// if

    if(repoMetaLink[1] != repoDataRight.repoMetaLink[1]) {

        return true;

    }// if

    if(repoMetaLink[2] != repoDataRight.repoMetaLink[2]) {

        return true;

    }// if

    if(repoGPGCheck[0] != repoDataRight.repoGPGCheck[0]) {

        return true;

    }// if

    if(repoGPGCheck[1] != repoDataRight.repoGPGCheck[1]) {

        return true;

    }// if

    if(repoGPGCheck[2] != repoDataRight.repoGPGCheck[2]) {

        return true;

    }// if

    if(repoGPGKey[0] != repoDataRight.repoGPGKey[0]) {

        return true;

    }// if

    if(repoGPGKey[1] != repoDataRight.repoGPGKey[1]) {

        return true;

    }// if

    if(repoGPGKey[2] != repoDataRight.repoGPGKey[2]) {

        return true;

    }// if

    if(repoProxy[0] != repoDataRight.repoProxy[0]) {

        return true;

    }// if

    if(repoProxy[1] != repoDataRight.repoProxy[1]) {

        return true;

    }// if

    if(repoProxy[2] != repoDataRight.repoProxy[2]) {

        return true;

    }// if

    if(repoProxyUsername[0] != repoDataRight.repoProxyUsername[0]) {

        return true;

    }// if

    if(repoProxyUsername[1] != repoDataRight.repoProxyUsername[1]) {

        return true;

    }// if

    if(repoProxyUsername[2] != repoDataRight.repoProxyUsername[2]) {

        return true;

    }// if

    if(repoProxyPassword[0] != repoDataRight.repoProxyPassword[0]) {

        return true;

    }// if

    if(repoProxyPassword[1] != repoDataRight.repoProxyPassword[1]) {

        return true;

    }// if

    if(repoProxyPassword[2] != repoDataRight.repoProxyPassword[2]) {

        return true;

    }// if

    if(repoBandwidth[0] != repoDataRight.repoBandwidth[0]) {

        return true;

    }// if

    if(repoBandwidth[1] != repoDataRight.repoBandwidth[1]) {

        return true;

    }// if

    if(repoBandwidth[2] != repoDataRight.repoBandwidth[2]) {

        return true;

    }// if

    if(repoSkipIfUnavailable[0] != repoDataRight.repoSkipIfUnavailable[0]) {

        return true;

    }// if

    if(repoSkipIfUnavailable[1] != repoDataRight.repoSkipIfUnavailable[1]) {

        return true;

    }// if

    if(repoSkipIfUnavailable[2] != repoDataRight.repoSkipIfUnavailable[2]) {

        return true;

    }// if

    if(repoFailoverMethod[0] != repoDataRight.repoFailoverMethod[0]) {

        return true;

    }// if

    if(repoFailoverMethod[1] != repoDataRight.repoFailoverMethod[1]) {

        return true;

    }// if

    if(repoFailoverMethod[2] != repoDataRight.repoFailoverMethod[2]) {

        return true;

    }// if

    if(repoMetadataExpire[0] != repoDataRight.repoMetadataExpire[0]) {

        return true;

    }// if

    if(repoMetadataExpire[1] != repoDataRight.repoMetadataExpire[1]) {

        return true;

    }// if

    if(repoMetadataExpire[2] != repoDataRight.repoMetadataExpire[2]) {

        return true;

    }// if

    if(repoParallelDownload[0] != repoDataRight.repoParallelDownload[0]) {

        return false;

    }// if

    if(repoParallelDownload[1] != repoDataRight.repoParallelDownload[1]) {

        return false;

    }// if

    if(repoParallelDownload[2] != repoDataRight.repoParallelDownload[2]) {

        return false;

    }// if

    if(repoInclude[0] != repoDataRight.repoInclude[0]) {

        return true;

    }// if

    if(repoInclude[1] != repoDataRight.repoInclude[1]) {

        return true;

    }// if

    if(repoInclude[2] != repoDataRight.repoInclude[2]) {

        return true;

    }// if

    if(repoExclude[0] != repoDataRight.repoExclude[0]) {

        return true;

    }// if

    if(repoExclude[1] != repoDataRight.repoExclude[1]) {

        return true;

    }// if

    if(repoExclude[2] != repoDataRight.repoExclude[2]) {

        return true;

    }// if

    return false;

}// operator !=
