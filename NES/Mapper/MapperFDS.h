#pragma once
#include "Mapper.h"
//////////////////////////////////////////////////////////////////////////
// Mapper020  Nintendo Disk System(FDS)                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper020 : public Mapper
{
public:
	Mapper020( NES* parent );

	void	Reset();

	BYTE	ExRead ( WORD addr );
	void	ExWrite( WORD addr, BYTE data );

	void	WriteLow( WORD addr, BYTE data );

	void	Write(WORD addr, BYTE data);

	void	Clock( INT cycles );
	void	HSync( INT scanline );
	void	VSync();

	BYTE	ExCmdRead ( EXCMDRD cmd );
	void	ExCmdWrite( EXCMDWR cmd, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	enum	{
		BLOCK_READY = 0,
		BLOCK_VOLUME_LABEL,
		BLOCK_FILE_AMOUNT,
		BLOCK_FILE_HEADER,
		BLOCK_FILE_DATA,
	};
	enum	{
		SIZE_VOLUME_LABEL = 56,
		SIZE_FILE_AMOUNT = 2,
		SIZE_FILE_HEADER = 16,
	};
	enum	{
		OFFSET_VOLUME_LABEL = 0,
		OFFSET_FILE_AMOUNT = 56,
		OFFSET_FILE_HEADER = 58,
		OFFSET_FILE_DATA = 74,
	};

	enum	{
		MECHANICAL_SOUND_BOOT = 0,
		MECHANICAL_SOUND_SEEKEND,
		MECHANICAL_SOUND_MOTOR_ON,
		MECHANICAL_SOUND_MOTOR_OFF,
		MECHANICAL_SOUND_ALLSTOP,
	};

	BOOL	bDiskThrottle;
	INT		DiskThrottleTime;

	LPBYTE	disk;
	LPBYTE	disk_w;

	INT		irq_counter, irq_latch;	// $4020-$4021
	BYTE	irq_enable, irq_repeat;	// $4022
	BYTE	irq_occur;		// #### When an interrupt occurs, the value should not be zero.
	BYTE	irq_transfer;		// #### Interrupt Transfer Flag

	BYTE	disk_enable;		// Disk I/O enable
	BYTE	sound_enable;		// Sound I/O enable
	BYTE	RW_start;		// #### Once the text-to-speech function is enabled, an IRQ (Interrupt Request) occurs.
	BYTE	RW_mode;		// #### writing mode
	BYTE	disk_motor_mode;	// #### Disk motor
	BYTE	disk_eject;		// #### Do not Insert CD
	BYTE	drive_ready;		// #### Are you in the middle of reading or writing?
	BYTE	drive_reset;		// #### Drive Reset State

	INT		block_point;
	INT		block_mode;
	INT		size_file_data;
	INT		file_amount;
	INT		point;
	BYTE	first_access;

	BYTE	disk_side;
	BYTE	disk_mount_count;

	BYTE	irq_type;

	// For mechanical sound
	BYTE	sound_startup_flag;
	INT		sound_startup_timer;
	INT		sound_seekend_timer;

	void	MechanicalSound( INT type );

private:
};
