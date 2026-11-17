from Bib1 import entrada

def operacao (x, y, op):
	# insert your code here
	
while (True):
	try:
		x = int(entrada())
		y = int(entrada())
		op = entrada()
	except:
		break
	print(operacao(x, y, op))