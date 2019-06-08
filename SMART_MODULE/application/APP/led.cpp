#include "common.h"

//LED gLed;

LED::LED_OPR_PATT_DEF l_patt_map[] = {	
/* obj,			action,				nstate,				delay,	utick, patt,	len,	idx */
{ new DigitalOut(LED_GRN, LOW), LED::eLED_OPR_RPT_IDLE, LED::eLED_OPR_STATE_OFF, 0, 0, NULL, 0, 0 },	//PB8, OUT
{ new DigitalOut(LED_RED, LOW), LED::eLED_OPR_RPT_IDLE, LED::eLED_OPR_STATE_OFF, 0, 0, NULL, 0, 0 },	//PB9, OUT
};

static void led_process();

LED::LED()
	: led_thread(osPriorityNormal, MBED_CONF_APP_THREAD_STACK_SIZE / 2)

{
	if (!sizeof(l_patt_map))
		return;
	
	led_thread.start(led_process);
}

LED::~LED()
{
	if (led_thread.get_state() >= Thread::State::Running)
		led_thread.terminate();
}

static void led_process()
{
	DBGMSG(0, "[LED] Process Started===>\r\n");
	gLed.bKeepRunning = true;
	wait_ms(100);
	while (gLed.bKeepRunning)
	{
		Thread::wait(100);
		for (int loop = 0; loop < sizeof(l_patt_map) / sizeof(l_patt_map[0]); loop++)
		{
			LED::LED_OPR_PATT_DEF* patt = &l_patt_map[loop];
			if (patt->action == LED::eLED_OPR_RPT_IDLE) continue;
			if (patt->pattern == NULL)
				switch (patt->action) {
				case LED::eLED_OPR_RPT_TRG:
					if (patt->delay != 0 && getElapsedTick(patt->utick) < (u32)patt->delay * 100)	continue;
					*patt->led = patt->nstate;
					patt->action = LED::eLED_OPR_RPT_IDLE;
					break;
				case LED::eLED_OPR_RPT_TRG_TOGG:
					if (patt->delay != 0 && getElapsedTick(patt->utick) < (u32)patt->delay * 100)	continue;
					{
						int state = patt->nstate;
						if (patt->patt_idx == 0 || patt->patt_idx == 2)
							state = (state == LED::eLED_OPR_STATE_OFF) ? LED::eLED_OPR_STATE_ON : LED::eLED_OPR_STATE_OFF;
						*patt->led = patt->nstate;
						patt->utick = getTickCount();
						patt->patt_idx++;
						break;
					}
				case LED::eLED_OPR_RPT_REPEAT:
					if (patt->delay == 0 || getElapsedTick(patt->utick) < (u32)patt->delay * 100)	continue;
					*patt->led = patt->nstate;
					patt->nstate = (patt->nstate == LED::eLED_OPR_STATE_OFF) ? LED::eLED_OPR_STATE_ON : LED::eLED_OPR_STATE_OFF;
					patt->utick = getTickCount();
					break;
				}
			else {
				switch (patt->action) {
				case LED::eLED_OPR_RPT_TRG:
					if (patt->patt_len == patt->patt_idx) {
						patt->action == LED::eLED_OPR_RPT_IDLE;
						continue;
					}
				case LED::eLED_OPR_RPT_REPEAT:
					if (getElapsedTick(patt->utick) < (u32)((u8*)patt->pattern)[patt->patt_idx] * 100)	continue;
					*patt->led = patt->nstate;
					patt->nstate = (patt->nstate == LED::eLED_OPR_STATE_OFF) ? LED::eLED_OPR_STATE_ON : LED::eLED_OPR_STATE_OFF;
					patt->utick = getTickCount();
					patt->patt_idx++;
					break;
				case LED::eLED_OPR_RPT_TRG_TOGG:
					break;
				}
				patt->patt_idx++;
			}
		}
	}
	DBGMSG(0, "[LED] Terminate\r\n");
}

void LED::SetPatt(u8 ledEnum, u8 action, u8 nstate, u8 delay, u8* pattSeq, u8 pattLen)
{
	LED_OPR_PATT_DEF* patt = &l_patt_map[ledEnum];
	patt->action = action;
	if (nstate < eLED_OPR_STATE_COUNT)
		patt->nstate = nstate;

	patt->delay = delay;
	if (patt->pattern != pattSeq || patt->patt_len != pattLen) {
		free(patt->pattern);
		patt->pattern = NULL;
	}
	if (patt->pattern == NULL && pattSeq != NULL)
		patt->pattern = malloc(pattLen);

	if (pattSeq != NULL) {
		memcpy(patt->pattern, pattSeq, pattLen);
	}
	patt->patt_len = pattLen;
	patt->patt_idx = 0;
	patt->utick = getTickCount();
}

void LED::SetGrpPatt(u32 ledMsk, u8 action, u8 nstate, u8 delay, u8* pattSeq, u8 pattLen)
{
	int ledShift = 0;
	do
	{
		if ((ledMsk & 1) == 0)
		{
			ledMsk = ledMsk >> 1;
			ledShift++;
			continue;
		}
		LED_OPR_PATT_DEF* patt = &l_patt_map[ledShift];
		patt->action = action;
		if (nstate < eLED_OPR_STATE_COUNT)
			patt->nstate = nstate;

		patt->delay = delay;
		if (patt->pattern != pattSeq || patt->patt_len != pattLen) {
			free(patt->pattern);
			patt->pattern = NULL;
		}
		if (patt->pattern == NULL && pattSeq != NULL)
			patt->pattern = malloc(pattLen);

		if (pattSeq != NULL) {
			memcpy(patt->pattern, pattSeq, pattLen);
		}
		patt->patt_len = pattLen;
		patt->patt_idx = 0;
		patt->utick = getTickCount();
		ledMsk = ledMsk >> 1;
		ledShift++;
	} while (ledShift < 31 && ledMsk > 0);
}
