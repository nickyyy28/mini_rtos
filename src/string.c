void memcpy(void *dest, void *src, unsigned int len)
{
	unsigned char *pcDest = dest;
	unsigned char *pcSrc  = src;
	
	while (len --)
	{
		*pcDest = *pcSrc;
		pcSrc++;
		pcDest++;
	}
}

void memset(void *dest, unsigned char val, unsigned int len)
{
	unsigned char *pcDest = dest;
	
	while (len --)
	{
		*pcDest = val;
		pcDest++;
	}
}