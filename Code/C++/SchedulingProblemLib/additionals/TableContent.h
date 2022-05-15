#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace scheduling_problem
{
	namespace additionals
	{
		template<class T>
		std::size_t size(T& elem)
		{
			std::stringstream s;
			s << elem;
			return s.str().size();
		}


		template<class IndexT, class NameT, class ContentT>
		class TableContent
		{

		private:

			template<class UnitNameT, class UnitContentT>
			class TableUnit
			{
			protected:
				UnitNameT name_;
				std::vector<UnitContentT> content_;
				std::size_t outputPadding_;

			public:
				TableUnit() : outputPadding_(0) {}

				TableUnit(const UnitNameT& unitName) : outputPadding_(0), name_(unitName) {}

				TableUnit(UnitNameT unitName, 
						  const std::vector<UnitContentT>& unitContent) 
						  : TableUnit(unitName)
				{
					content_ = std::vector<UnitContentT>(unitContent.size());
					std::transform(unitContent.begin(), unitContent.end(), content_.begin(), [](auto elem) { return elem; });
				}

				TableUnit(const TableUnit& unit)
				{
					name_ = unit.name_;
					content_ = unit.content_;
					outputPadding_ = unit.outputPadding_;
				}

				void append(const UnitContentT& value)
				{
					content_.push_back(value);
					this->padding();
				}

				std::size_t size() const
				{
					return content_.size();
				}

				typename std::vector<UnitContentT>::const_iterator begin()
				{
					return content_.begin();
				}

				typename std::vector<UnitContentT>::const_iterator end()
				{
					return content_.end();
				}

				UnitNameT name() const
				{
					return name_;
				}

				UnitContentT& operator[](int index)
				{
					return content_[normalizeIndex(index)];
				}

				const UnitContentT operator[](int index) const
				{
					return content_[normalizeIndex(index)];
				}

			protected:
				std::size_t normalizeIndex(int index) const
				{
					if (index < 0 && -index <= content_.size()) {
						return content_.size() + index;
					}
					if (index >= 0 && index < content_.size()){
						return index;
					}
					throw;
				}

				virtual void padding()
				{
					auto len = scheduling_problem::additionals::size(operator[](-1));
					if (len > outputPadding_) {
						outputPadding_ = len;
					}
				}

				friend std::ostream& operator<<(std::ostream& strm, const TableUnit& unit)
				{
					strm << std::setw(unit.outputPadding_) << unit.name();
					for (int i(0); i < unit.size(); i++) {
						strm << "\n" << std::setw(unit.outputPadding_) <<  unit[i];
					}
					return strm;
				}
			};

			typedef TableUnit<IndexT, ContentT> Row;			

		public:
			std::vector<NameT> columns;
		private:
			std::unordered_map<NameT, int> columnsMap_;
			std::unordered_map<IndexT, std::size_t> rowsMap_;
			std::vector<Row> rows_;
			std::pair<std::size_t, std::size_t> shape_;
			std::vector<std::size_t> rowPadding_;
			std::size_t indexColPadding_;

		public:
			TableContent() {}

			TableContent(const std::vector<NameT>& columns) : TableContent()
			{
				int i = 0;
				for (auto& col : columns) {
					columnsMap_[col] = i++;
				}
				shape_ = std::pair<std::size_t, std::size_t>(0, columns.size());
				std::transform(columns.begin(), columns.end(), std::back_inserter(rowPadding_),
					[](auto name) { return scheduling_problem::additionals::size(name); });
				for (auto col : columns) {
					this->columns.push_back(col);
				}
			}

			TableContent(const TableContent& table) 
			{
				columns = table.columns;
				columnsMap_ = table.columnsMap_;
				shape_ = table.shape_;
				rowPadding_ = table.rowPadding_;
				indexColPadding_ = table.indexColPadding_;
				rows_ = table.rows_;
			}

			void append(const IndexT& name, const std::vector<ContentT>& content)
			{
				if (content.size() != shape_.second) {
					throw;
				}
				rowsMap_[name] = rows_.size();
				rows_.push_back(Row(name, content));
				updatePadding(name, content);
			}

			void updatePadding(const IndexT& name, const std::vector<ContentT>& content)
			{
				std::size_t len;
				for (std::size_t i(0); i < content.size(); i++) {
					if ((len = scheduling_problem::additionals::size(content[i])) > rowPadding_[i]) {
						rowPadding_[i] = len;
					}
				}

				if ((len = scheduling_problem::additionals::size(name)) > indexColPadding_) {
					indexColPadding_ = len;
				}
			}

			Row& operator[](const IndexT& name)
			{
				return rows_[rowsMap_[name]];
			}

			ContentT& operator[](const std::pair<const IndexT&, const NameT&>& place)
			{
				return rows_[rowsMap_[place.first]][columnsMap_[place.second]];
			}

			void to_csv(std::string filepath, char sep = ';')
			{
				std::ofstream file;
				file.open(filepath);
				for (std::size_t i(0); i < columns.size(); i++) {
					file << sep << columns[i];
				}
				file << std::endl;
				for (auto& row : rows_) {
					file << row.name();
					for (int i(0); i < row.size(); i++) {
						file << sep << row[i];
					}
					file << std::endl;
				}
				file.close();
			}

			friend std::ostream& operator<<(std::ostream& strm, const TableContent& table)
			{
				strm << std::setw(table.indexColPadding_) << "";
				for (std::size_t i(0); i < table.columns.size(); i++) {
					strm << " " << std::setw(table.rowPadding_[i]) << table.columns[i];
				}
				strm << std::endl;
				for (auto& row : table.rows_) {
					strm << std::setw(table.indexColPadding_) << row.name() << " ";
					for (int i(0); i < row.size(); i++) {
						strm << std::setw(table.rowPadding_[i]) << row[i] << " ";
					}
					strm << std::endl;
				}
				return strm;
			}
		};

	}
}

