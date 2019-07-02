#pragma once

QString stdStringToQString(const std::string & str);

std::string qStringToStdString(const QString & str);

std::wstring mbs_to_wcs(std::string const& str, std::locale loc);

std::string wcs_to_mbs(std::wstring const& str, std::locale loc);