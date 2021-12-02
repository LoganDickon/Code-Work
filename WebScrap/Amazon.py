import xlsxwriter
from bs4 import BeautifulSoup
from msedge.selenium_tools import Edge, EdgeOptions
from selenium import webdriver

def get_url(search_term):
	template = "https://www.amazon.com/s?k={}&ref=nb_sb_noss_2"
	search_term = search_term.replace(' ', '+')

	# add term query to url
	url = template.format(search_term)

	# add page query placeholder
	url += '&page{}'

	return template.format(search_term)

def extract_record(item):
	#Extract and return all item info

	# Items Description
	atag = item.h2.a
	description = atag.text.strip()

	# Items URL
	url = 'https://www.amazon.com' + atag.get('href')

	try:
		# Items Price
		price_parent = item.find('span', 'a-price')
		price = price_parent.find('span', 'a-offscreen').text
	
	except AttributeError:
		return

	try:
		# Items Ratings
		rating = item.i.text
		review_count = item.find('span', {'class' : 'a-size-base'}).text
	
	except AttributeError:
		rating = ''
		review_count = ''

	result = (description, price, rating, review_count, url)

	return result

def main(search_term):
	# Start Web Driver
	driver = webdriver.Edge('C:/WebScraping-main/distro/msedgedriver.exe')
	options = EdgeOptions()
	options.use_chromium = True

	records = []
	url = get_url(search_term)

	for page in range(1, 21):
		driver.get(url.format(page))
		soup = BeautifulSoup(driver.page_source, 'html.parser')
		results = soup.find_all('div', {'data-component-type': 's-search-result'})

		for item in results:
			record = extract_record(item)
			if record:
				records.append(record)

	driver.close()

	row = 1
	col = 0
	
	workbook = xlsxwriter.Workbook('C:/WebScraping-main/Scraped Files/' + (str(search_term) + '.xlsx').lower())
	worksheet = workbook.add_worksheet()
	worksheet.merge_range(0,0 , 0,8, 'Product')
	worksheet.merge_range(0,9 , 0,10, 'Price $')
	worksheet.merge_range(0,11 , 0,12, 'Rating')
	worksheet.merge_range(0,13 , 0,14, 'Num of Reviews')
	worksheet.write(0, 15, 'URL')

	for rec, rec2, rec3, rec4, rec5 in records:
		worksheet.set_row(row, 25)
		worksheet.merge_range(row, col, row, col+8, rec)		#(start_row, start_col, end_row, end_col, item)
		worksheet.merge_range(row, col+9, row, col+10, rec2)
		worksheet.merge_range(row, col+11, row, col+12, rec3)
		worksheet.merge_range(row, col+13, row, col+14, rec4)
		worksheet.write(row, col + 15, rec5)
		row += 1

                
	workbook.close()

print("Welcome! This program takes any product search on Amazon.com and extracts the data to an Excel spreadsheet format(.xlsx)")
print("Note! What you type here will be the name of the file!")
Search = input('Enter Search: ')
main(Search)
