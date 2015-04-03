#ifndef TDSBUFFER_HPP
#define TDSBUFFER_HPP

#include <string>

#include "event.hpp"

namespace tdcdata {

/**
 * @class DataSet
 * @author
 * @date 03/15/15
 * @file tudataset.hpp
 * @brief Буфер событий TDSEvent
 */
class DataSet : public std::vector<TUEvent> {
  public:
	/**
	 * @brief Считывание из директории или файла
	 * @param path Имя директории или файла
	 */
	void read(const std::string& path);
	/**
	 * @brief Добавление в буфер данных из директории или файла
	 * @param path Имя файла или директории
	 */
	void append(const std::string& path);
	/**
	 * @brief Нахождение множества сработваших дрейфовых камер
	 * @return Множество сработваших дрейфовых камер
	 */
	std::set<uintmax_t> getTriggeredChambers();
	/**
	 * @brief Нахождение множества сработваших дрейфовых камер
	 * @param triggChams Множество сработваших дрейфовых камер
	 */
	void loadTriggeredChambers(std::set<uintmax_t>& triggChams);
	/**
	 * @brief Проверка расширения файла
	 * @return true - если файл является TUDataSet
	 *		   false - если файл не является TUDataSet
	 */
	static bool checkExtension(const std::string& fileName);
  protected:
	void	 deserializeStream (std::istream& stream);
	uint32_t convert(uint32_t word , double koef) const;
	void	 parseStream (std::istream& stream, size_t streamSize);
	void	 convert(UIntVector& vec, double koef) const;
  private:
	TUDataSetHeader mCurrentHeader;
};

} //tudata

#endif // TDSBUFFER_HPP
