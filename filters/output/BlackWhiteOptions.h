/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

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

#ifndef OUTPUT_BLACK_WHITE_OPTIONS_H_
#define OUTPUT_BLACK_WHITE_OPTIONS_H_

class QString;
class QDomDocument;
class QDomElement;

namespace output
{

enum BinarizationMethod { OTSU, SAUVOLA, WOLF };

class BlackWhiteOptions
{
public:
	BlackWhiteOptions();
	
	BlackWhiteOptions(QDomElement const& el);
	
	QDomElement toXml(QDomDocument& doc, QString const& name) const;
	
	int thresholdAdjustment() const { return m_thresholdAdjustment; }
	void setThresholdAdjustment(int val) { m_thresholdAdjustment = val; }

	BinarizationMethod binarizationMethod() const { return m_binarizationMethod; }
	void setBinarizationMethod(BinarizationMethod method) { m_binarizationMethod = method; }

	int windowSize() const { return m_windowSize; }
	void setWindowSize(int size) { m_windowSize = size; }

	double sauvolaCoef() const { return m_sauvolaCoef; }
	void setSauvolaCoef(double k) { m_sauvolaCoef = k; }

	int wolfLowerBound() const { return m_wolfLowerBound; }
	void setWolfLowerBound(int val) { m_wolfLowerBound = val; }
	int wolfUpperBound() const { return m_wolfUpperBound; }
	void setWolfUpperBound(int val) { m_wolfUpperBound = val; }
	double wolfCoef() const { return m_wolfCoef; }
	void setWolfCoef(double k) { m_wolfCoef = k; }
		
	bool operator==(BlackWhiteOptions const& other) const;
	bool operator!=(BlackWhiteOptions const& other) const;
private:
	static BinarizationMethod parseBinarizationMethod(QString const& str);
	static QString formatBinarizationMethod(BinarizationMethod method);

	int m_thresholdAdjustment;
	BinarizationMethod m_binarizationMethod;
	int m_windowSize;
	double m_sauvolaCoef;
	int m_wolfLowerBound;
	int m_wolfUpperBound;
	double m_wolfCoef;
};

} // namespace output

#endif
